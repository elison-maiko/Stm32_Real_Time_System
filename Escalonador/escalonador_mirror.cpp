/****************************************************************************
* MInimal Real-time Operating System (MiROS), GNU-ARM port.
* version 1.26 (matching lesson 26, see https://youtu.be/kLxxXNCrY60)
*
* This software is a teaching aid to illustrate the concepts underlying
* a Real-Time Operating System (RTOS). The main goal of the software is
* simplicity and clear presentation of the concepts, but without dealing
* with various corner cases, portability, or error handling. For these
* reasons, the software is generally NOT intended or recommended for use
* in commercial applications.
*
* Copyright (C) 2018 Miro Samek. All Rights Reserved.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.
*
* Git repo:
* https://github.com/QuantumLeaps/MiROS
****************************************************************************/
#include <stdint.h>
#include "miros.h"
#include "qassert.h"
#include "stm32f1xx.h"
#include <stddef.h>

Q_DEFINE_THIS_FILE

OSThread * volatile OS_curr; /* pointer to the current thread */
OSThread * volatile OS_next; /* pointer to the next thread to run */

OSThread *OS_thread[32 + 1]; /* array of threads started so far */
uint32_t OS_readySet; /* bitmask of threads that are ready to run */
uint32_t OS_delayedSet; /* bitmask of threads that are delayed */

#define LOG2(x) (32U - __builtin_clz(x))

OSThread idleThread;
void main_idleThread() {
    while (1) {
        OS_onIdle();
    }
}

void OS_init(void *stkSto, uint32_t stkSize) {
    /* set the PendSV interrupt priority to the lowest level 0xFF */
    *(uint32_t volatile *)0xE000ED20 |= (0xFFU << 16);

    /* start idleThread thread */
    OSThread_start(&idleThread,
                   0U, /* idle thread priority */
                   &main_idleThread,
                   stkSto, stkSize);
}

// --------------------------- IMPLEMENTATION RMA ------------------------------

int is_schedulable_RTA(TaskParamets tasks[], uint8_t Ntasks) {
    for (int i = 0; i < Ntasks; i++) {
        uint32_t W = tasks[i].cost_abs;                         // Tempo de resposta inicial é igual ao custo
        uint32_t W_last = 0;

        while (W != W_last) {                                   // Iterativamente calcular W até que ele converja
            W_last = W;
            W = tasks[i].cost_abs;                                  // Inicia com o custo da tarefa
            for (int j = 0; j < i; j++)                         // Somar a interferência das tarefas de maior prioridade
                W += (W_last / tasks[j].period_abs) * tasks[j].cost_abs;
            if (W > tasks[i].period_abs)                            // Verifica se o tempo de resposta excede o período (não escalonável)
                return 0;                                       // Não escalonável
        }
    }
    return 1;  // Todas as tarefas são escalonáveis
}
//void(void){}

// ------------------------- END ESPECIFIC FUNCTIONS --------------------------

void OS_sched(void) {
    /* Escolhe a próxima thread para executar */
    OSThread *next;
    if (OS_readySet == 0U) { /* condição de idle? */
        next = OS_thread[0]; /* a thread de idle */
    }
    else {
        uint32_t bit = OS_readySet & -OS_readySet;    // Isola o bit 1 mais à direita (Maior prioridade é o menor período)
        next = OS_thread[LOG2(bit)];                  // Obtém a thread correspondente
        Q_ASSERT(next != (OSThread *)0);
    }
    /* Trigger PendSV, se necessário */
    if (next != OS_curr) {
        OS_next = next;                     // Atualiza a variável global next caso seja diferente da atual
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // PendSV para mudar contexto
        __asm volatile("dsb");
        // __asm volatile("isb");
    }
    /*
     * DSB - whenever a memory access needs to have completed before program execution progresses.
     * ISB - whenever instruction fetches need to explicitly take place after a certain point in the program,
     * for example after memory map updates or after writing code to be executed.
     * (In practice, this means "throw away any prefetched instructions at this point".)
     * */
}

void OS_run(void) {
    /* callback to configure and start interrupts */
    OS_onStartup();

    __disable_irq();

    OS_sched();
    __enable_irq();

    /* the following code should never execute */
    Q_ERROR();
}

void OS_tick(void) {                                            //Decrementa o que tá em delay
    uint32_t workingSet = OS_delayedSet & -OS_delayedSet;       // ----------------------------------------------------- Mexendo na Delayset
    while (workingSet != 0U) {                                  //Enquanto tiver tarefa em delay
        OSThread *tdelay = OS_thread[LOG2(workingSet)];         //Pega a mais a esquerda
        uint32_t bit;
        Q_ASSERT((tdelay != (OSThread *)0) && (tdelay->paramets.period_relative != 0U));

        bit = (1U << (tdelay->prio - 1U));                      // Desloca 1 para esquerda (t->prio) casas
        --tdelay->paramets.period_relative;                     // Decrementa o paramets.period_relative da tarefa t = workset = tarefa no OSdelayed

        if(tdelay->paramets.cost_relative > 0){
            tdelay->paramets.cost_relative--;
        }
        if (tdelay->paramets.period_relative == 0U) {           // Se o paramets.period_relative zerar
            OS_readySet   |= bit;  /* insert to set */          // Põe ela no pronto
            OS_delayedSet &= ~bit; /* remove from set */        // Retira ela do Delay
        }
        workingSet &= ~bit; /* remove from working set */       // Para Redefinir o workingset
    }

    uint32_t tasks = OS_readySet;                               // ------------------------------------------------------ Mexendo na Readyset
    while (tasks != 0U) {
        OSThread *tready = OS_thread[LOG2(tasks)];              // Obtém a próxima thread a partir do bit set mais alto

        if (tready->index == OS_curr->index){                   // Se a tarefa está em execução, decrementa o custo
            if (tready->paramets.cost_relative > 0)             // E se o custo não zerou, decremeta o custo
                tready->paramets.cost_relative--;
        }
        if (tready->paramets.period_relative > 0) {             // Decremento do período
                tready->paramets.period_relative--;
            }
        if (tready->paramets.period_relative == 0){             // Reajuste de periodo e custo relativo, como absoluto novamente, ao fim do periodo relativo
            tready->paramets.period_relative = tready->paramets.period_abs;
            tready->paramets.cost_relative = tready->paramets.cost_abs;
        }
        tasks &= ~(1U << (tready->index - 1U));                 // Atualiza a máscara para verificar a próxima tarefa (verificar adição a direita ou a esquerda)
    }
}

//n É usado nessa implementaçãp
void OS_delay_RM(uint32_t ticks) {
    uint32_t bit;
    __asm volatile ("cpsid i");

    /* never call OS_delay from the idleThread */
    Q_REQUIRE(OS_curr != OS_thread[0]);

    OS_curr->paramets.period_relative = ticks;
    bit = (1U << (OS_curr->prio - 1U));
    OS_readySet &= ~bit;
    OS_delayedSet |= bit;
    OS_sched();
    __asm volatile ("cpsie i");
}

void OSThread_start(
    OSThread *me,
    uint8_t prio, /* thread priority */
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize)
{
    /* round down the stack top to the 8-byte boundary
    * NOTE: ARM Cortex-M stack grows down from hi -> low memory
    */
    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    uint32_t *stk_limit;

    /* priority must be in ragne
    * and the priority level must be unused
    */
    Q_REQUIRE((prio < Q_DIM(OS_thread))
              && (OS_thread[prio] == (OSThread *)0));

    *(--sp) = (1U << 24);  /* xPSR */
    *(--sp) = (uint32_t)threadHandler; /* PC */
    *(--sp) = 0x0000000EU; /* LR  */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3  */
    *(--sp) = 0x00000002U; /* R2  */
    *(--sp) = 0x00000001U; /* R1  */
    *(--sp) = 0x00000000U; /* R0  */
    /* additionally, fake registers R4-R11 */
    *(--sp) = 0x0000000BU; /* R11 */
    *(--sp) = 0x0000000AU; /* R10 */
    *(--sp) = 0x00000009U; /* R9 */
    *(--sp) = 0x00000008U; /* R8 */
    *(--sp) = 0x00000007U; /* R7 */
    *(--sp) = 0x00000006U; /* R6 */
    *(--sp) = 0x00000005U; /* R5 */
    *(--sp) = 0x00000004U; /* R4 */

    /* save the top of the stack in the thread's attibute */
    me->sp = sp;

    /* round up the bottom of the stack to the 8-byte boundary */
    stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1U) / 8) + 1U) * 8);

    /* pre-fill the unused part of the stack with 0xDEADBEEF */
    for (sp = sp - 1U; sp >= stk_limit; --sp) {
        *sp = 0xDEADBEEFU;
    }

    /* register the thread with the OS */
    OS_thread[prio] = me;
    me->prio = prio;
    /* make the thread ready to run */
    if (prio > 0U) {
        OS_readySet |= (1U << (prio - 1U));
    }
}

// ------------------------------ SEMAPHORE ------------------------------

void sem_init(semaphore_t *p_sem, uint8_t valor_init) {
	Q_ASSERT(p_sem != NULL); //Verifica se o ponteiro é válido ou nulo
	p_sem->valor_sem = valor_init;
	/* valor_init da criação do semaforo é atribuido a variavel valor_sem, da struct*/
}

void sem_post(semaphore_t *p_sem){
	__disable_irq();
	p_sem->valor_sem++; /*Incremento entre interrupção, para proteçao*/
	__enable_irq();
}

void sem_wait(semaphore_t *p_sem){
	__disable_irq();
	while(p_sem->valor_sem == 0){ /*Enquanto o valor do semaforo for 0 não é possivel decrementar*/
		__enable_irq();
		OS_delay(1U); /*Reabilita a interrupção e gera delay para outra thread incrementar o valor do semaforo*/
		__disable_irq(); /*disabilita interrupção para outra verificação do while*/
	}
	/*Assim que valor_sem > 0, ocorre o decremento*/
	p_sem->valor_sem --;
	__enable_irq();
}

// --------------------------- END SEMAPHORE -----------------------------

__attribute__ ((naked, optimize("-fno-stack-protector")))
void PendSV_Handler(void) {
__asm volatile (

    /* __disable_irq(); */
    "  CPSID         I                 \n"

    /* if (OS_curr != (OSThread *)0) { */
    "  LDR           r1,=OS_curr       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  CBZ           r1,PendSV_restore \n"

    /*     push registers r4-r11 on the stack */
    "  PUSH          {r4-r11}          \n"

    /*     OS_curr->sp = sp; */
    "  LDR           r1,=OS_curr       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  STR           sp,[r1,#0x00]     \n"
    /* } */

    "PendSV_restore:                   \n"
    /* sp = OS_next->sp; */
    "  LDR           r1,=OS_next       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  LDR           sp,[r1,#0x00]     \n"

    /* OS_curr = OS_next; */
    "  LDR           r1,=OS_next       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  LDR           r2,=OS_curr       \n"
    "  STR           r1,[r2,#0x00]     \n"

    /* pop registers r4-r11 */
    "  POP           {r4-r11}          \n"

    /* __enable_irq(); */
    "  CPSIE         I                 \n"

    /* return to the next thread */
    "  BX            lr                \n"
    );
}
