# Stm32_Real_Time_System
Disciplina de Engenharia Mecatrônica
    EMB5633-06605 (20242) - Sistemas de Tempo Real

Discente: Elison Maiko Oliveira de Souza
Matrícula: 22102900

# MIROS - Sistema Operacional em Tempo Real com Rate Monotonic (RM)

Este repositório contém a implementação de um sistema operacional básico para microcontroladores em tempo real, com suporte para escalonamento usando o algoritmo Rate Monotonic (RM) e semáforos para controle de sincronização entre tarefas.

## Estrutura do Projeto

- **TaskParamets**: Estrutura para armazenar os parâmetros de tempo de execução e período de cada tarefa, necessária para o escalonamento RM.
- **OSThread**: Bloco de Controle de Thread (TCB) que mantém o estado de cada tarefa, incluindo prioridade, timeout e uma pilha própria.
- **Funções de Escalonamento**:
  - **is_schedulable_RTA()**: Função que verifica se as tarefas são escalonáveis com base na Análise de Tempo de Resposta.
  - **OS_calculate_next_periodic_task_RM()**: Função que determina a próxima tarefa a ser executada com base no escalonamento RM.
- **Semáforos**: Estruturas e funções para controle de sincronização entre tarefas.

## Principais Funcionalidades

### Escalonador Rate Monotonic

O escalonador RM é baseado em prioridades fixas, atribuídas de acordo com o período de cada tarefa (tarefas com menor período têm maior prioridade). Esta abordagem é útil para sistemas com tarefas periódicas críticas que devem ser executadas em intervalos regulares.

- **Função de escalonabilidade**: `is_schedulable_RTA()` garante que o conjunto de tarefas seja escalonável antes de iniciar o SO.
- **Cálculo de tarefas periódicas**: `OS_calculate_next_periodic_task_RM()` realiza a seleção da próxima tarefa a ser executada com base nas prioridades estabelecidas.

### Controle de Semáforos
O sistema oferece suporte a semáforos binários para sincronização entre tarefas. Cada semáforo tem uma estrutura `semaphore_t` que mantém seu valor atual, e funções para inicializar (`sem_init()`), sinalizar (`sem_post()`), e aguardar (`sem_wait()`) o semáforo.

### Funções do SO

- **OS_init**: Inicializa o sistema operacional.
- **OS_run**: Inicia o SO, permitindo que as threads comecem a executar.
- **OS_delay**: Função de atraso para gerenciamento de tempo de execução de cada tarefa.
- **OS_tick**: Atualiza o contador de tempo do SO, acionando tarefas conforme o tempo limite.

## Como Usar

1. **Configuração**: Inclua `MIROS.h` no seu projeto e configure o tempo do tick (`TICKS_PER_SEC`) de acordo com as especificações de hardware.
2. **Inicialize o SO**: Use `OS_init()` para configurar a pilha e `OS_run()` para iniciar a execução das threads.
3. **Defina Tarefas**: Inicialize cada thread com `OSThread_start()` e defina os parâmetros no bloco `TaskParamets`.
4. **Controle de Semáforos**: Use `sem_init()`, `sem_wait()` e `sem_post()` para gerenciar sincronização entre tarefas.

## Exemplo de Código