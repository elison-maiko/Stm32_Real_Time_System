# Stm32_Real_Time_System##
Disciplina de Engenharia Mecatrônica
    EMB5633-06605 (20242) - Sistemas de Tempo Real

Discente: Elison Maiko Oliveira de Souza
Matrícula: 22102900

---

## Sobre as Alterações no Código
Inicialmente, foram declaradas na mirroh.h quatro novas funções:

- 1. `struct semaphore_t`
    Contém apenas uma definição em seu escopo, referente ao valor inicial do semáforo.
	uint8_t valor_sem;

- 2. `sem_init(semaphore_t *p_sem, uint8_t valor_init)`
    Responsável pela iniciação do semáforo.

- 3. `sem_post(semaphore_t *p_sem)`
    Responsável por adicionar itens ao buffer, referente ao produtor.

- 4. `sem_wait(semaphore_t *p_sem)`
    Responsável por retirar itens do buffer, referente ao consumidor.

---

## Implementação das Funções de Semáforo
Em seguida, a implementação das três funções do semáforo foi realizada na mirror.c:

- 1. `sem_init(semaphore_t *p_sem, uint8_t valor_init)`
    Define o valor inicial do semáforo, que será utilizado para controlar a quantidade de recursos disponíveis. Isso é fundamental para garantir que o sistema comece com o estado correto.

- 2. `sem_post(semaphore_t *p_sem)`
    Essa função é chamada pelo produtor. Ela incrementa o valor do semáforo, indicando que um recurso (ou item no buffer) foi produzido e está disponível para o consumidor. A função garante que a operação ocorra sem interrupções ao desabilitar as interrupções com __disable_irq() enquanto incrementa o valor do semáforo e reabilita logo após a operação com __enable_irq().

- 3. `sem_wait(semaphore_t *p_sem)`
    Essa função é chamada pelo consumidor. Ela decrementa o valor do semáforo, indicando que um recurso foi consumido. Caso o valor do semáforo seja zero, o consumidor entra em espera até que o produtor adicione um novo recurso. Assim como no sem_post(), as interrupções são desabilitadas ao realizar a operação para garantir a integridade dos dados. A função também realiza um bloqueio, forçando a thread a esperar (com OS_delay()) enquanto o valor do semáforo é zero.

---

## Estrutura do Código

### Constantes

- `bufferCapacity`: Define a capacidade máxima do buffer (25).
- `initial_Cons`: Define o valor inicial de consumo (0).
- `prod_Time`: Tempo necessário para produzir um item (40).
- `cons_Time`: Tempo necessário para consumir um item (80).
- `breaktime_Cons`: Intervalo entre consumos (100).
- `breaktime_Prod`: Intervalo entre produções (70).

### Variáveis Globais

- `semaphore_t buffer_prod`: Semáforo para controlar o espaço disponível para produção.
- `semaphore_t buffer_cons`: Semáforo para controlar os itens prontos para consumo.

### Funções

#### `produtor()`

- Simula a função do produtor.
- Utiliza `sem_wait(&buffer_prod)` para aguardar espaço disponível no buffer.
- Chama `OS_delay(prod_Time)` para simular o tempo de produção.
- Usa `sem_post(&buffer_cons)` para sinalizar que um item foi produzido.
- Aguarda um intervalo fixo antes de produzir o próximo item.

#### `consumidor()`

- Simula a função do consumidor.
- Utiliza `sem_wait(&buffer_cons)` para aguardar um item disponível.
- Chama `OS_delay(cons_Time)` para simular o tempo de consumo.
- Usa `sem_post(&buffer_prod)` para sinalizar que um espaço foi liberado.
- Aguarda um intervalo fixo antes de consumir o próximo item.

#### `main()`

- Inicializa a pilha do thread inativo.
- Chama `OS_init()` para inicializar o sistema operacional.
- Inicializa os semáforos `buffer_prod` e `buffer_cons` com seus respectivos valores.
- Cria as threads para o produtor e o consumidor.
- Inicia o sistema operacional com `OS_run()`.
