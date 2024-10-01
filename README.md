# Stm32_Real_Time_System
Disciplina de Engenharia Mecatrônica
    EMB5633-06605 (20242) - Sistemas de Tempo Real

Discente: Elison Maiko Oliveira de Souza
Matrícula: 22102900

Sobre as alterações no código:

Inicialmente foi declarado na mirroh.h, 4 funções novas: 

struct semaphore_t: contento apenas uma definição em seu escopo, referente ao valor inicial do semaforo

sem_init(semaphore_t *p_sem, uint8_t valor_init): 
    Responsável pela iniciação do semáforo

sem_post(): 
    Responsável por adicionar itens ao buffer, referente ao produtor

sem_wait():
    Responsável por retirar itens do buffer, referente ao consumidor   

Em seguida a implementação das 3 funções do semáforo foram realizadas na mirror.c:


sem_init(semaphore_t *p_sem, uint8_t valor_init):
     Define o valor inicial do semáforo, que será utilizado para controlar a quantidade de recursos disponíveis. Isso é fundamental para garantir que o sistema comece com o estado correto.

sem_post(semaphore_t *p_sem):
    Essa função é chamada pelo produtor. Ela incrementa o valor do semáforo, indicando que um recurso (ou item no buffer) foi produzido e está disponível para o consumidor.
    A função garante que a operação ocorra sem interrupções ao desabilitar as interrupções __disable_irq() enquanto incrementa o valor do semáforo e reabilitar logo após a operação com __enable_irq().

sem_wait(semaphore_t *p_sem):
    Essa função é chamada pelo consumidor. Ela decrementa o valor do semáforo, indicando que um recurso foi consumido. Caso o valor do semáforo seja zero, o consumidor entra em espera até que o produtor adicione um novo recurso.
    Assim como no sem_post(), as interrupções são desabilitadas ao realizar a operação para garantir a integridade dos dados. A função também realiza um bloqueio, forçando a thread a esperar (OS_delay()) enquanto o valor do semáforo é zero.

Threads Produtor-Consumidor
    No conceito de produtor-consumidor, duas threads trabalham em conjunto para gerenciar um buffer de recursos:

    Produtor: Cria novos recursos e usa sem_post() para indicar que o recurso está disponível no buffer.

    Consumidor: Consome os recursos e utiliza sem_wait() para garantir que haja recursos disponíveis antes de consumir.
