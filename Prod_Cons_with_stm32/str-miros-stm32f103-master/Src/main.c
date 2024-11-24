#include <stdlib.h>
#include <stdio.h>
#include "miros.h"

#define STACK_SIZE 40  /* Tamanho da pilha para cada thread */
#define C1 200
#define T1 800
#define C2 200
#define T2 600
#define BSPRIO 4

/** Estrutura de controle de thread **/
struct threads {
    OSThread threadCB;
    uint32_t stack[STACK_SIZE];
};


semaphore_t Setor1;
semaphore_t Setor2;


void task1() {
    while(1) {
        current_task = 1;
        // Simula trabalho da task
        for(volatile int i = 0; i < C1; i++) {
            __asm("nop");
        }
    }
}

/** Função que simula um consumidor **/
void consumidor() {
    while (1) {
    }
}

struct threads produtorThread;
struct threads consumidorThread;

int main() {
    uint32_t idleStack[STACK_SIZE];  /* Pilha do thread inativo */

    /** Inicializa o sistema operacional **/
    OS_init(idleStack, sizeof(idleStack));

    /** Inicializa os semáforos **/
    sem_init(&buffer_prod, 1);  /* Espaço disponível no início */
    sem_init(&buffer_cons, 1);  /* Nenhum item pronto inicialmente */

    /* Inicializa a thread do produtor*/
    OSThread_start(&(produtorThread.threadCB), 1, &produtor, produtorThread.stack, sizeof(produtorThread.stack));
    OSThread_start(&(consumidorThread.threadCB), 2, &consumidor, consumidorThread.stack, sizeof(consumidorThread.stack));

    OS_run();
}

