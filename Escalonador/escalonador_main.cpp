#include <stdint.h>
#include <stdlib.h>
#include "miros.h"

#define STACK_SIZE 40  /* Tamanho da pilha para cada thread */
#define N_TAREFAS 3

/** Estrutura de controle de thread **/
struct threads {
    OSThread threadCB;
    uint32_t stack[STACK_SIZE];
};

uint8_t now = 0;

/* Definindo as funções específicas para cada tarefa */
void task1() {
    while(1) {
        now = 1;
    }
}

void task2() {
    while(1) {
        now = 2;
    }
}

void task3() {
    while(1) {
        now = 3;
    }
}

int main() {
    uint32_t idleStack[STACK_SIZE];  /* Pilha do thread inativo */
    struct threads tasks[N_TAREFAS];

    /* Tarefas com custos e períodos diferentes*/
    uint32_t tarefas[N_TAREFAS][2] = {
        {20, 60}, // T1 Custo e Período
        {20, 80}, // T2  '' 
        {20, 100} // T3  '' 
    };
    /*
    uint32_t periodos[] = {tarefas[0][1], tarefas[1][1], tarefas[2][1]}; // Exemplo de períodos
    uint32_t mdc, mmc;
    calcular_mdc_mmc(periodos, 3, &mdc, &mmc);
    //O QUE FAZER COM O MMC MESMO>>>>>>>>>>>>>>>>> FAZER RTA DIRETO É MAIS EFICIENTE
    */

    /* Funções de tarefas associadas*/
    void (*taskFunctions[N_TAREFAS])() = {task1, task2, task3}; // Gambiarra para apontar taskfunction[i] para task, Funciona>>>>>

    for (int i = 0; i < N_TAREFAS; i++) {
        tasks[i].threadCB.TaskParamets.cost_abs = tarefas[i][0];     // Custo
        tasks[i].threadCB.TaskParamets.cost_relative = tarefas[i][0];     // Custo
        tasks[i].threadCB.TaskParamets.period_abs = tarefas[i][1];   // Período
        tasks[i].threadCB.TaskParamets.period_relative = tarefas[i][1];   // Período
    }

    /* Verifica se as tarefas são escalonáveis usando o RTA */
    if (!is_schedulable_RTA(tasks, N_TAREFAS)) {
        return 1;                                // Encerrar o programa se não for escalonável
    }

    /* Inicia cada tarefa */
    for (int i = 0; i < N_TAREFAS; i++) {
        OSThread_start(&tasks[i].threadCB,       // Controle da thread
                       i,                        // Prioridade da thread
                       taskFunctions[i],         // Função da tarefa
                       tasks[i].stack,           // Pilha da thread
                       sizeof(tasks[i].stack));  // Tamanho da pilha
    }         

    OS_run();
}

/*
Resta faazer:
    Diminuição dos custos e periodos relativos no tick
    definir o mmc como tempo de execução
    redefinir custo e periodo relativo como absoluto assim que zerar


*/

