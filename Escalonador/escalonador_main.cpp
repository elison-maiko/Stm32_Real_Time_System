#include <stdint.h>
#include <stdlib.h>
#include "miros.h"

#define STACK_SIZE 40  /* Tamanho da pilha para cada thread */

/** Estrutura de controle de thread **/
struct threads {
    OSThread threadCB;
    uint32_t stack[STACK_SIZE];
};


//
int main() {
    uint32_t idleStack[STACK_SIZE];  /* Pilha do thread inativo */

    /** Inicializa o sistema operacional **/
    OS_init(idleStack, sizeof(idleStack));

    /* Inicializa a thread do produtor*/
    OSThread_start(&(produtorThread.threadCB), 1, &produtor, produtorThread.stack, sizeof(produtorThread.stack));

    /* Inicializa a thread do consumidor*/
    OSThread_start(&(consumidorThread.threadCB), 2, &consumidor, consumidorThread.stack, sizeof(consumidorThread.stack));

    OS_run();
}

