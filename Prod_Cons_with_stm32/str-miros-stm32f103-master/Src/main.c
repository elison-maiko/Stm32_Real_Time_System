#include <stdlib.h>
#include <stdio.h>
#include "miros.h"

#define STACK_SIZE 40  /* Tamanho da pilha para cada thread */
#define C1 200
#define T1 800
#define C2 200
#define T2 600

uint32_t visual_paramet = 0;
uint32_t atual_task = 0;
semaphore recurso;


void task1() {
    while(1) {
        sem_wait(&recurso);
        atual_task = 1;                        // Indica que task 1 está escalonada
        for(volatile vizual = 0; i < C1; i++) {// Simula trabalho da task
            __asm("nop");
        }
        sem_post(&recurso);
        wait_next_period();
    }
}
void task2() {
    while(1) {
        sem_wait(&recurso);
        atual_task = 2;                        // Indica que task 1 está escalonada
        for(volatile vizual = 0; i < C2; i++) {// Simula trabalho da task
            __asm("nop");
        }
        sem_post(&recurso);
        wait_next_period();
    }
}
void APtask() {
    while(1) {
        sem_wait(&recurso);
        atual_task = 10;                        // Indica que task 1 está escalonada
        for(volatile vizual = 0; i < 1000; i++) {// Simula trabalho da task
            __asm("nop");
        }
        sem_post(&recurso);      
    }
}

int main() {
    uint32_t idleStack[STACK_SIZE];  /* Pilha do thread inativo */

    /** Inicializa o sistema operacional **/
    OS_init(idleStack, sizeof(idleStack));

    threads thread_task1;
    threads thread_task2;
    threads thread_taskAP;

    /* Inicializa a threads Periodicas -->> Ajustar OSCREAT*/
    OSThread_start_P(&thread_task1.threadCB, &task1, C1, T1,thread_task1.stack, sizeof(thread_task1.stack));
    OSThread_start_P(&thread_task2.threadCB, &task2, C2, T2,thread_task2.stack, sizeof(thread_task2.stack));
    
    /** Inicializa os semáforos **/
    sem_init(&recurso 1, 1);  

    /* Inicializa a threads Periodicas -->> Ajustar OSCREAT*/


    OS_run();
}

