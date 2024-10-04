# Stm32_Real_Time_System##
Disciplina de Engenharia Mecatrônica
    EMB5633-06605 (20242) - Sistemas de Tempo Real

Discente: Elison Maiko Oliveira de Souza
Matrícula: 22102900

---

## Sobre as Alterações no Código
Inicialmente, foram declaradas na mirroh.h quatro novas funções:

 ### 1. struct semaphore_t
    Contém apenas uma definição em seu escopo, referente ao valor inicial do semáforo.

### 2. sem_init(semaphore_t *p_sem, uint8_t valor_init)
    Responsável pela iniciação do semáforo.

### 3. sem_post()
    Responsável por adicionar itens ao buffer, referente ao produtor.

### 4. sem_wait()
    Responsável por retirar itens do buffer, referente ao consumidor.

---

## Implementação das Funções de Semáforo
Em seguida, a implementação das três funções do semáforo foi realizada na mirror.c:

### 1. sem_init(semaphore_t *p_sem, uint8_t valor_init)
Define o valor inicial do semáforo, que será utilizado para controlar a quantidade de recursos disponíveis. Isso é fundamental para garantir que o sistema comece com o estado correto.

### 2. sem_post(semaphore_t *p_sem)
Essa função é chamada pelo produtor. Ela incrementa o valor do semáforo, indicando que um recurso (ou item no buffer) foi produzido e está disponível para o consumidor. A função garante que a operação ocorra sem interrupções ao desabilitar as interrupções com __disable_irq() enquanto incrementa o valor do semáforo e reabilita logo após a operação com __enable_irq().

### 3. sem_wait(semaphore_t *p_sem)
Essa função é chamada pelo consumidor. Ela decrementa o valor do semáforo, indicando que um recurso foi consumido. Caso o valor do semáforo seja zero, o consumidor entra em espera até que o produtor adicione um novo recurso. Assim como no sem_post(), as interrupções são desabilitadas ao realizar a operação para garantir a integridade dos dados. A função também realiza um bloqueio, forçando a thread a esperar (com OS_delay()) enquanto o valor do semáforo é zero.
