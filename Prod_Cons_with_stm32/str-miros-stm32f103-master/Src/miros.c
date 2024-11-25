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

OSThread *OS_thread[32 + 1]; /* array of periodic threads started so far */
OS_thread *OS_ThreadAP[32+1];/* array of aperiodic threads started so far */
uint32_t OS_readySet;        /* bitmask of periodic threads that are ready to run */
uint32_t OS_delayedSet;      /* bitmask of periodic threads that are delayed */
uint32_t OS_readySetAP;      /* bitmask of aperiodic threads that are ready to run */
uint8_t N_tasks_P = 0;       
uint8_t N_tasks_AP = 0;

#define LOG2(x) (32U - __builtin_clz(x))
#define LOG2RIGTH(x) (__builtin_clz(x))

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
    OSThread_start_P(&idleThread,
                   0U, /* idle thread priority */
                   &main_idleThread,
                   stkSto, stkSize);
}

void OS_sched(void) {
    /* choose the next thread to execute... */
    OSThread *next;
    index_periodic = LOG2(OS_readySet);
    index_aperiodic = LOG2(OS_readySetAP);
    if (index_periodic == 0U) {                     /* idle condition? -> Sem tarefa periodicas prontas */
        if(index_aperiodic == 0U)                   /* Verificar tarefas aperiodicas prontas */
            next = OS_threadAP[index_aperiodic];    /* Escaolona tarefa aperiodica pronta, apontando para o bitmask aperiodico com lista aperiodica */
        next = OS_thread[0];                        /* the idle thread */
    }
    else {
        next = OS_thread[index_periodic];
        Q_ASSERT(next != (OSThread *)0);
    }

    /* trigger PendSV, if needed */
    if (next != OS_curr) {
        OS_next = next;
        //*(uint32_t volatile *)0xE000ED04 = (1U << 28);
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
        __asm volatile("dsb");
//        __asm volatile("isb");
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

void OS_wait_next_period(){
    __disable_irq();
    uint8_t bit = (1U << (OS_curr->prio - 1U));
    OS_readySet   &= ~bit;                      /* insert to set */
    OS_delayedSet |= bit;                       /* remove from set */
    OS_sched();
    __enable_irq();
}

/* Função de atualização de parametos relativos */
void att_paramets(OSThread *ME){
    ME->Paramets.cost_relative = Me->Paramets.cost_abs;
    Me->Paramets.period_relative = ME->Paramets.period_abs;
}
/* Função de Inserção de tasks com prioridade RM */
void add_task(OSThread *ME) {
    uint8_t inserted = 0;
    /* CORAÇÃO DO ESCALONADOR RM */
    /* IDLE */
    if (N_tasks_P == 0) {
    OS_tasks[0] = ME;
    OS_tasks[0]->prio = 0;  
    } 
    else {
        uint8_t inserted = 0;
        for (uint8_t i = 0; i < N_tasks_P; i++) {                               /* Itera pelas tarefas já existentes para encontrar a posição correta */
            if (OS_tasks[i]->Paramets.period_abs > ME->Paramets.period_abs) {   /* Verifica se o período absoluto da nova tarefa é menor que o da tarefa i */
                for (uint8_t j = N_tasks_P; j > i; j--) {                       
                    OS_tasks[j] = OS_tasks[j - 1];                              /* Move as tarefas existentes para a direita para abrir espaço para a nova tarefa */
                    OS_tasks[j]->prio = j;                                      /* Atualiza a prioridade das tarefas movidas */
                }
                OS_tasks[i] = ME;                                               /* Insere a nova tarefa na posição correta */
                OS_tasks[i]->prio = i;                                          /* Define a prioridade da nova tarefa (posição i) */
                inserted = 1;                                                   /* Marca que a tarefa foi inserida */
                break;                                                          /* Sai do loop, pois a tarefa já foi inserida */
            }
        }
        /* Caso a nova tarefa tenha o maior período, ela é inserida no final da lista */
        if (!inserted) {
            OS_tasks[N_tasks_P] = ME;
            OS_tasks[N_tasks_P]->prio = N_tasks_P;                              /* Prioridade igual à posição (última posição) */
        }
    }
}
void OS_tick(void) {
    /* Função Original pra decremento no delayset */
    uint32_t workingSet = OS_delayedSet;
    while (workingSet != 0U) {
        OSThread *t = OS_thread[LOG2(workingSet)];
        uint32_t bit;
        Q_ASSERT((t != (OSThread *)0) && (t->timeout != 0U));

        bit = (1U << (t->prio - 1U));
        --t->timeout;
        if (t->timeout == 0U) {
            OS_readySet   |= bit;   /* insert to set */
            OS_delayedSet &= ~bit;  /* remove from set */
        }
        workingSet &= ~bit;         /* remove from working set */
    }
    /* Decremento de variaveis da thread*/
    for (uint8_t i = 0; i < N_tasks_P; i++){
        OS_thread *task = OS_thread[i+1];
        task->Paramets.period_relative --;
        task->Paramets.cost_relative ---;           /* Será q preciso desse cara ? */
        if (task->Paramets.cost_relative == 0 ){    /* Teoricamente, no pior dos casos o periodo e o custo finalizando juntos, considerando deadline=periodo */
            uint32_t bit = (1U << (t->prio - 1U));  /* Mas se o custo acabar antes do periodo */
            OS_readySet   |= bit;                   /* insert to set */
            OS_delayedSet &= ~bit;      /* remove from set */
            att_custos(task);
        }
    }
}

void OS_delay(uint32_t ticks) {
    uint32_t bit;
    __asm volatile ("cpsid i");

    /* never call OS_delay from the idleThread */
    Q_REQUIRE(OS_curr != OS_thread[0]);

    OS_curr->timeout = ticks;
    bit = (1U << (OS_curr->prio - 1U));
    OS_readySet &= ~bit;
    OS_delayedSet |= bit;
    OS_sched();
    __asm volatile ("cpsie i");
}

/* Receber Periodo e Converter Periodo em PRIO*/
void OSThread_start_P(
    OSThread *me,
    OSThreadHandler threadHandler,
    uint32_t custo,
    uint32_t periodo,
    void *stkSto, uint32_t stkSize)
{
    /* round down the stack top to the 8-byte boundary
    * NOTE: ARM Cortex-M stack grows down from hi -> low memory
    */
    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    uint32_t *stk_limit;
    
    /* Ntasks  tem q ser menor q o tamanho de Os_tasks eeeee  O espaço do indice deve estar vazio*/
    Q_REQUIRE((N_tasks_P + 1 < Q_DIM(OS_tasks) - 1) && (OS_tasks[N_tasks_P + 1] == (OSThread *)0));

    if (threadHandler != &main_idleThread) 
        N_tasks_P++;

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

    me->Paramets.cost_abs = custo;
    me->Paramets.cost_relative = custo;
    att_paramets(me);
    /* Chamada da função que adiciona a task no seu lugar devido */
    add_task(me);

    /* make the thread ready to run */
    if (me->prio > 0U) {
        OS_readySet |= (1U << (me->prio - 1U));
    }
}

void OSThread_start_AP(OSThread *me,
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize){

    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    uint32_t *stk_limit;

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

    OS_ThreadAP[N_tasks_AP] = me;
    OS_ThreadAP[N_tasks_AP]->prio = N_tasks_AP;
    OS_ThreadAP[N_tasks_AP]->setor_critico = N_tasks_AP;

    N_tasks_AP++;

    if (me->prio > 0U) {
        OS_readySetAP |= (1U << (me->prio - 1U));       /* Lansa a tredi ap nu bit mascarado */
    }
}

void sem_init(semaphore *p_sem, uint8_t valor_init, uint8_t max) {
	Q_ASSERT(p_sem != NULL);
	p_sem->valor_sem = valor_init;      /* valor_init da criação do semaforo é atribuido a variavel valor_sem, da struct*/
    p_sem->max_buffer = max;            /* valor maximo do buffer */
}

void sem_post(semaphore *p_sem){
	__disable_irq();
	p_sem->valor_sem++; // AJEITAR ESTRUTURA DE NPP
	__enable_irq();
}

void sem_wait(semaphore *p_sem){
	__disable_irq();
	while(p_sem->valor_sem <= 0){ 
		__enable_irq();
		OS_delay(1U);     // AJUSTAR LOGICA DE  NPP
		__disable_irq(); 
	}
	p_sem->valor_sem --;
	__enable_irq();
}


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
