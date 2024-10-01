#include <stdint.h>
#include <stdlib.h>
#include "miros.h"

#define STACK_SIZE 40  /* Tamanho da pilha para cada thread */

/** Estrutura de controle de thread **/
struct threads {
    OSThread threadCB;
    uint32_t stack[STACK_SIZE];
};

/** Constantes relacionadas à produção e consumo **/
const uint8_t bufferCapacity = 25;              /* Valor de início para produção */
const uint8_t initial_Cons = 0;                 /* Valor inicial para consumo */
const uint8_t prod_Time = 40;                   /* Tempo de produção */
const uint8_t cons_Time = 80;                   /* Tempo de consumo */
const uint8_t breaktime_Cons = 100;             /* Intervalo entre consumos */
const uint8_t breaktime_Prod = 70;              /* Intervalo entre produções */

semaphore_t buffer_prod;  /* Espaço disponível para produzir */
semaphore_t buffer_cons;  /* Itens prontos para consumir */

/** Função que simula um produtor **/
void produtor() {
    while (1) {
        sem_wait(&buffer_prod);       /* Aguarda espaço disponível no buffer */
        OS_delay(prod_Time);          /* Simula o tempo necessário para produzir */
        sem_post(&buffer_cons);       /* Sinaliza que um item foi produzido */
        OS_delay(breaktime_Prod);     /* Espera um tempo para produzir o próximo */
    }
}

/** Função que simula um consumidor **/
void consumidor() {
    while (1) {
        sem_wait(&buffer_cons);       /* Aguarda um item disponível para consumo */
        OS_delay(cons_Time);          /* Simula o tempo necessário para consumir */
        sem_post(&buffer_prod);       /* Sinaliza que um espaço foi liberado no buffer */
        OS_delay(breaktime_Cons);     /* Espera um tempo fixo antes de consumir o próximo */
    }
}

int main() {
    uint32_t idleStack[STACK_SIZE];  /* Pilha do thread inativo */

    /** Inicializa o sistema operacional **/
    OS_init(idleStack, sizeof(idleStack));

    /** Inicializa os semáforos **/
    sem_init(&buffer_prod, bufferCapacity);  /* Espaço disponível no início */
    sem_init(&buffer_cons, initial_Cons);    /* Nenhum item pronto inicialmente */

    /** Criação de apenas um produtor e um consumidor para simplificar **/

    struct threads produtorThread;
    struct threads consumidorThread;

    /* Inicializa a thread do produtor*/
    OSThread_start(&(produtorThread.threadCB), 1, &produtor, produtorThread.stack, sizeof(produtorThread.stack));

    /* Inicializa a thread do consumidor*/
    OSThread_start(&(consumidorThread.threadCB), 2, &consumidor, consumidorThread.stack, sizeof(consumidorThread.stack));

    OS_run();
}

