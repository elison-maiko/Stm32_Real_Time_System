# Sistemas de tempo real (STR)

[TOC]

## Read Me

Criei esse documento para servir como um guia completo da teoria estudada nas aulas de Sistemas de tempo real. A idéia não é ser um Resumão, e sim, abordar cada detalhe para a compreensão do assunto.

O estudo deve ser complementado com a lista de exercícios do Butazzo e os exemplos dos Slides do professor. Capítulos recomendados: 2, 4, 5 e 7.

O arquivo "Todo o conteúdo teórico.md" foi feito com Markdow, um dos melhores programas para fazer a leitura desse tipo de arquivo é o Typora, nele você pode ativar para interagir com as formulas criadas em LaTeX. Ou, também é possível exportar para PDF. 

*Copyright &copy; 2019, Paulo Victor Duarte, All rights reserved.*

# Formulário

## Testes

Teste Suficiente:  $Ub=\sum \frac{Ci}{Ti} < n(2^{1/n}-1)$

Teste Hyperbolic Bound: $\prod (Ui +1) < 2$

Teste Exato: $Ri = \sum \left \lceil \frac{Ri}{Tk} \right \rceil Ck + Ci \leq Di$

## Testes com Bi

Teste Approach-First: $\sum \frac{Ck}{Tk} +\frac{Ci+Bi}{Ti} \leq n(2^{1/n}-1) $

Teste RTA: $Ri = \sum \left \lceil \frac{Ri}{Tk} \right \rceil Ck + Ci + Bi \leq Di$

## Utilização dos Protocolos Aperiódicos

Utilização por PS: $U_{PS}\leq 2(\frac{Up}{n}+1)^{-n}-1$

Utilização por DS: $U_{DS}\leq  \frac{2-k}{2k-1}$ e $K=(\frac{Up}{n}+1)^{n}$

Utilização por SS: $U_{SS}\leq 2(\frac{Up}{n}+1)^{-n}-1$

# Introdução

## Real Time Operating System 
O conceito mais importante de um sistema de tempo real : *Previsibilidade*
É capacidade de verificar, em tempo de projeto, se os processos irão perder o deadline.
O que um *RTOS* deve garantir
+ Ordenar tarefas
+ Ativar tarefas prontas
+ Resolver conflitos de recursos compartilhados (Exclusão Mútua)
+ Gerenciar execução oportuna de eventos assíncronos (por interrupção)
  
### Soft RT
+ Perdas de deadline não são catastróficas para o sistema
+ Erros podem ocorrer e a saída ainda é aceitável
+ Analisa o tempo para o caso médio
  
### Hard RT
+ A perda de deadline não é uma opção
+ Necessário garantir requisitos temporais em projeto
+ Analisa o tempo para o pior caso (maior atraso)
  
### Firm RT
+ A perda de deadline não causa danos ao sistema, mas a saída é descartada
  (como um sensor que faz uma leitura errada, a resposta é descartada)

## Tipos de tarefas
1. *Aperiódicas*: 
	+ Não há regras para o tempo de execução
	+  Nada se sabe sobre a ativação das tarefas

2. *Periódicas*: 
	+ Todas devem ser executadas
	+ A tarefa é ativada a cada $Ti $ unidades de tempo 
	+ O instante de chegada pode ser calculado a partir do início

3. *Esporádicas*: 
	+ Instantes de chegada não são conhecidos
	+ Existe um intervalo mínimo de tempo entre chegadas 
	+ É uma tarefa aperiódica que se comporta como o pior caso de uma tarefa periódica

## Nomenclatura

### Especificados pelo programa
1. $Ci$: Tempo de computação
3. $Ti$: Período para a execução
4. $Di$: Deadline Relativo

### Depende do escalonador e da execução
1. $ai$: tempo de chegada
2. $si$: start time
3. $fi$: finish time
4. $ri$: tempo de resposta
5. $Slack$: tempo de sobra entre $fi$ e $di$
6. $Lateness$: tempo de atraso entre $fi$ e $di$
7. $Jitter$: tempo perdido para a troca de contexto entre uma saída de tarefa em execução e o início da outra
   
### Deadlines
1. $ Di $: Deadline Relativo, em relação ao início da tarefa ( Concluir em 10 ms )
2. $ di $: Deadline Absoluto, em relação à linha do tempo ( Concluir até as 10:00:00 )

### Formulas
Tempo de resposta = $fi-ai$
Tempo de atraso ( + Slack) = $fi-di$
Tempo de atraso ( - Lateness) = $fi-di$

## Definição de preemptivo e não preemptivo
Um escalonador é considerado preemptivel se a execução de uma tarefa pode ser arbitrariamente suspensa a qualquer momento para atribuir a CPU à outra tarefa. Em um escalonador não preeptivo a tarefa em execução não pode ser interrompida até que esteja concluída.

## Testes

### Suficiente
+ O teste suficiente verifica se o sistema é escalonável através de testes simples de escalonamento.

+ *Teste de Utilização*
	
	+ > $\lim_{n \to \infty}\frac{Ci}{Ti} = Log(2) = 0.693$
	+ > $Ub=\sum \frac{Ci}{Ti} < n(2^{1/n}-1)$

+ *Hyperbolic Bound*

	+ > $\prod (Ui +1) < 2$

### Exato
+ O teste exato verifica com algoritmo RTA de forma exata se o sistema é escalonável
+ *Teste de RTA* 
	
	+ Analisa a maior interferência que a tarefa pode sofrer pelo sistema
	+ A ordem de chegada das tarefas deve seguir a escolha do escalonador em teste
	+ Este teste deve prosseguir  para cada tarefa até que Ri se repita e seja menor que Di
	+ Há casos onde $Di=Ti$
	+ $Ri = \sum \left \lceil \frac{Ri}{Tk} \right \rceil Ck + Ci \leq Di$
	+ A propriedade de teto ( $\left \lceil \ X \right \rceil$ ), deve ser arredondado para o próximo número inteiro.
	+ Ex: $\left \lceil \ 1,06 \right \rceil = 2$
	> $R1^{0} = C1 \leq D1$
	> $R2^{0} = C1 + C2 \leq D2$
	> $R2^{1} = \left \lceil \frac{R2^{0}}{T1} \right \rceil C1 + C2 \leq D2$
	> $R3^{0} = C1 + C2 + C3 \leq D3$
	> $R3^{1} = \left \lceil \frac{R3^{0}}{T1} \right \rceil C1 + \left \lceil \frac{R3^{0}}{T2} \right \rceil C2 + C3 \leq D3$
	> $R3^{2} = \left \lceil \frac{R3^{1}}{T1} \right \rceil C1 + \left \lceil \frac{R3^{1}}{T2} \right \rceil C2 + C3 \leq D3$

### Necessário
+ O teste Necessário verifica em condições reais a escalonabilidade, pois há casos que o sistema é escalonável mas não há como provar.

## Conceitos importantes

*Static*: Prioridades estáticas são determinadas em projeto e nunca mudam.

*Dynamic*: Priodidades dinâmicas mudam em tempo de execução.

*Off-line*: Um escalonador off-line tem pré determinado o tempo de chegada de cada tarefa em projeto.

*On-line*: Um escalonador on-line tem o comportamento imprevisível de chegada das tarefas e escalona as mesmas durante a execução.

*Optimal*: Um algoritimo de escalonamento é dado como ótimo se atende o melhor cenário em que está sendo feito o teste.

## Escalonadores de tempo real

+ Best effort
+ Dynamic Guarantees
+ Desing-time guarantees
	+ Cyclic Executive
	+ Priority Driven
	    + Static Priority (RM & DM)
	    + Dynamic Priority (EDF)
  
## Uniprocessadores RTS 
1. *Table driven Scheduling*: a tabela dita quando cada tarefa irá executar
2. *Fixed-Priority Scheduling*: cada tarefa possui uma prioridade fixa
3. *Dynamic-Priority Scheduling*: a prioridade varia na execução

# Escalonadores

## Abordagens com garantia em projeto (Static Priority)

Oferece previsibilidade determinística, com análise feita em projeto
*Atribuir*: Atribuir propriedades para cada tarefa de acordo com a política de escalonamento
*Verificar*: Verificar a viabilidade de acordo com os testes
*Executar*: executar as tarefas de acordo com as prioridades

| Vantagens                                         | Desvantagens                                  |
| ------------------------------------------------- | --------------------------------------------- |
| Determina em projeto o cumprimento dos deadlines  | Necessário conhecer a carga                   |
| Necessária para aplicações criticas               | Necessário reservar recursos para o pior caso |
| Teoria serve de base para abordagens sem garantia | Difícil de determinar o pior caso             |

### Executivo Cíclico
Todo o escalonador é feito em projeto.
Resultado em grade da execução (determina a tarefa a ser executada).
Já foi usado em sistemas militares, navegação e monitoramento de sistemas.
Só funciona para tarefas periódicas e análise off-line.
*Vantagem*: comportamento conhecido.
*Desvantagem*: Escalonamento muito rígido, grade muito grande.

+ O tempo é dividido em intervalos iguais (Slots)
+ Cada tarefa é alocada em um Slot para atender a solicitação desejada
+ O Slot de execução é acionado por um timer

Ferramentas necessárias para criar um executivo cíclico
1. Interrupção
2. Timer
3. Laço FOR
4. Defines do minor e major cycle

| Vantagens                             | Desvantagens                                                 |
| ------------------------------------- | ------------------------------------------------------------ |
| Simples implementação                 | Fragilidade com overloads, efeito dominó                     |
| Baixa sobrecarga do tempo de execução | Dificuldade de expandir o escalonador para casos específicos |
| Baixo Jiter                           | Dificuldade em lidar com tarefas aperiódicas                 |

plain
╔═════════╦════╦═════╗
║ Tarefas ║ Ci ║  Ti ║
╠═════════╬════╬═════╣    
║    A    ║ 10 ║  25 ║   Minor Cycle = 25  
╠═════════╬════╬═════╣   Major Cycle = 100
║    B    ║ 10 ║  50 ║
╠═════════╬════╬═════╣
║    C    ║ 10 ║ 100 ║
╚═════════╩════╩═════╝


### Rate Monotonic (RM)
+ Prioridade mais alta para a tarefa com o menor período $(Ti)$
+ Prioridade fixa
+ Teste suficiente (Utilização e Hyperbolic Bound)
+ Teste exato (RTA)

### Deadline Monotonic (DM)
+ Prioridade mais alta para a tarefa com o menor deadline relativo $(Di)$
+ Prioridade fixa
+ Quanto menor o deadline, maior a prioridade.
+ Igual o RM quando $Ti=Di$
+ Teste suficiente (Utilização e Hyperbolic Bound)
+ Teste exato (RTA)

## Abordagens de prioridade dinâmica (Dynamic Priority )

### Earliest Deadline First (EDF)
+ Prioridade inversamente proporcional ao deadline absoluto
+ A tarefas que estiver com seu deadline mais próximo de terminar é priorizada.
+ Não é utilizado em sistemas críticos, pois seu *comportamento é imprevisível*. Sistemas críticos utilizam RM, DM e Executivo cíclico.
+ Ótimo em relação aos critérios de prioridade variáveis. Se o EDF não conseguir escalonar, nenhum outro algoritmo irá conseguir.
+ Teste suficiente $Ub=\sum \frac{Ci}{Ti} < 1$

### Least Slack Fisrt (LSF)
+ Maior prioridade às tarefas com maior tempo livre
+ Ótimo em relação aos critérios de prioridade variáveis
+ Overhead maior que EDF
+ Para tarefas em execução a prioridade se mantém
+ Tarefas em Ready aumentam a prioridade
## RM x EDF
+ RM é previsível.
+ RM é mais fácil de implementar, basta fixar prioridades. O EDF precisa recalcular a prioridade durante a execução.
+ Se o sistema está sobrecarregado, o RM garante que as tarefas de maior prioridade serão pouco afetadas.
+ Há mais testes de análise necessária para o RM
+ O RM é melhor para identificar o motivo na ocorrência de erros (Ex: Overloads)

---

# Servidores Aperiódicos
1. Sempre deve ser garantido os deadlines
2. O que sobra é fornecido aos Servidores Aperiódicos
3. Os jobs aperiódicos são executados em fila pelo servidor
4. Duas filas de processos no escalonador ( Periódicas e Aperiódicas )

## Background Server (BS)
+ Só permite a execução de tarefas aperiódicas se não houver nenhuma outra tarefa periódica em execução. Opera em espaços livres.
+ Executa enquanto o processador estiver em idle
+ Apenas para tarefas aperiódicas sem restrição de tempo.

## Poolling Server (PS)
+ O comportamento da tarefa servidor é igual a uma periódica
+ Possui um período $ Ti$ e um tempo de execução $Ci$ (Carga do Budget)
+ No pior caso a interferência do PS no sistema é igual a uma tarefa periódica
+ No período da chegada de uma tarefa aperiódica, mesmo que reste tempo para sua execução, ela só irá iniciar sua execução no próximo período.
+ Server Budget:  Recarga ocorre a cada período apenas se houver jobs aperiodicos.
+ Máxima utilização do servidor para o sistema ser escalonável
  + $U_{PS}\leq 2(\frac{Up}{n}+1)^{-n}-1$

## Deferable Server (DS)
+ Possui um período $ Ti$ e um tempo de execução $Ci$ (Carga do Budget), normalmente com maior prioridade.
+ Não possui o mesmo comportamento de uma tarefa periódica
  + Nem sempre executa no seu tempo de chegada mesmo com maior prioridade
+ A tarefa é iniciada imediatamente dentro do período em que ocorreu.
+ Server Budget:  Recarga ocorre a cada período, mesmo quando não existem jobs aperiódicos.
+ Máxima utilização do servidor para o sistema ser escalonável
  +   $U_{DS}\leq  \frac{2-k}{2k-1}$ e $K=(\frac{Up}{n}+1)^{n}$

## Sporadic Server (SS)
+ O teste de garantia é o mesmo do PS
+ Se comporta como uma tarefa esporádica
+ Possui um período $ Ti$ e um tempo de execução $Ci$ (Carga do Budget)
+ Preserva a capacidade e difere na forma como é feita a recarga
+ Para determinar a Recarga são feitas duas perguntas 
  + $(RT)$ Quando? :  A recarga ocorre em $RT=Ti+Ts$  e $Ts$ é onde a tarefa foi iniciada.
  + $(RA)$ Quanto? :   A recarga fornecida é a mesma consumida pela tarefa dentro do período ativado.
+ Máxima utilização do servidor para o sistema ser escalonável
  +  $U_{SS}\leq 2(\frac{Up}{n}+1)^{-n}-1$

# Acesso a Sessões críticas

## Inversão de prioridade
+ Ocorre quando uma tarefa de maior prioridade é bloqueada durante sua execução por uma tarefa de menor prioridade que possui um recurso compartilhado pelo mesmo semáforo. Neste cenário a tarefa de menor prioridade deve executar até liberar o recurso.
+ Outro cenário ocorre quando temos três tarefas. A t1 é bloqueada por t3 pois compartilham recursos, porém com a chegada de t2, que ganha prioridade de execução, e impede t1 de sair da região crítica. Por consequência, t1 ficará bloqueada no durante toda a execução de t2 e até t3 liberar o recurso.
  
## Non Preemptive Protocol (NPP)
+ Aumenta a prioridade dinâmica de uma tarefa em execução que utilize um recurso compartilhado (região crítica).
+ Evita o problema da inversão de prioridades
+ Qualquer tarefa pode ser bloqueada por uma de menor prioridade.
+ Causa bloqueio desnecessário:  Uma tarefa está bloqueando o uso da CPU por operar na região critica, enquanto tarefas de maior prioridade ficam bloqueadas mesmo não usando o recurso.
+ Outro problema do NPP é que para ser implementado é necessário desabilitar as interrupções. Neste caso não há como tarefas de maior prioridade preeptarem o processo durante um acesso a região crítica.

## Priority inheritance Protocol (PIP)
+ Herança de prioridade. A tarefa de menor prioridade herda a de maior prioridade
+ A inversão de prioridade descontrolada é evitada.
+ Não impede *DEADLOCKS*
+ A prioridade só é aumentada durante a região crítica
+ Propriedade transitiva ( se t2 aumentar a prioridade, t3 também aumenta).
+ Regra de herança de prioridade
	+ Quando a terefa tenta alocar o recurso R e fica bloqueado
	+ O job Jx que detém o recurso R herda a prioridade efetiva de J
	+ Jx mantém esta prioridade até o momento que libera o recurso R
	+ Neste instante ele retorna para sua prioridade efetiva anterior
	+ A herança de prioridade é transitiva
+ Possui prioridade Ativa e Nominal
	+ Prioridade Ativa (p): definida durante a execução.
	+ Prioridade Nominal (P): definida em tempo de projeto.
+ Tipos de bloqueio
  + Direto: t2 bloqueia t1
  + Transitivo: t3 bloqueia t1,  e também bloqueia t2 por herança de prioridade

## Priotity Ceiling Protocol (PCP)
+ No PCP a herança é dada quando uma tarefa de maior prioridade é bloqueada. 
+ Elimina a possibilidade de ocorrer *DEADLOCKS*
+ Ceiling C(Sk) prioridade C no Semáforo do recurso K
+ O valor de C(Sk) é o mesmo da tarefa de maior prioridade que utiliza o recurso K.
+ O Ceiling é conhecido antes de o programa começar a executar. É estático e nunca muda.
+ A prioridade tem que ser maior que a C, para receber a vez de execução.
+ Se $Pi\leq C(Sk)$  o acesso é negado.
+ A propriedade Ceiling faz com que cada tarefa seja bloqueada por no máximo uma vez
+ Assume-se que se sabe quais recursos cada tarefa precisa
+ Mesmo um recurso livre pode não ser alocado !!
  + Feito para prevenir um comportamento pior
+ Tipos de bloqueio
  + Direto: t2 bloqueia t1
  + Transitivo: t3 bloqueia t1,  e também bloqueia t2 por herança de prioridade
  + Teto: O recurso de t1 está livre, mas mesmo assim ela é bloqueada por t3  por prioridade de Ceiling (teto).
+ Quem tem a posse do recurso tem sempre a mais alta prioridade relativa
+ Quem tem o recurso é ativado se não existir uma tarefa de prioridade maior

## Immediate priority ceiling protocol (IPCP)
+ No IPCP a herança é dada IMEDIATAMENTE no início da sessão critica
+ A tarefa bloqueia imediatamente quando possui o recurso e executa com prioridade de teto
+ A tarefa $tj$ só é bloqueada pela tarefa $ti$ se $Pj<Pi\leqslant C(Sk)$
+ O bloqueio máximo é o mesmo do PCP
+ Tipos de bloqueio
  + Direto: t2 bloqueia t1
  + Transitivo: t3 bloqueia t1,  e também bloqueia t2 por herança de prioridade
  + Teto: O recurso de t1 está livre, mas mesmo assim ela é bloqueada por t3  por prioridade de Ceiling (teto).

# Teste de escalonabilidade para $Bi$
Quando implementamos um protocolo de segurança para o acesso a regiões críticas cada tarefa terá um tempo quem que permanece bloqueada ($Bi$). Neste caso, utilizamos os algoritmos abaixo para verificar se o sistema é escalonável.

## Teste Approach First
$\sum \frac{Ck}{Tk} +\frac{Ci+Bi}{Ti} \leq n(2^{1/n}-1) $
> #### $\frac{C1+B1}{T1}  \leq 1(2^{1/1}-1) $
> #### $\frac{C1}{T1} + \frac{C2+B2}{T2}  \leq 2(2^{1/2}-1) $
> #### $\frac{C1}{T1} + \frac{C2}{T2} + \frac{C3+B3}{T3}  \leq 3(2^{1/3}-1)$

## Teste RTA
Este teste deve prosseguir  para cada tarefa até que Ri se repita e seja menor que Di
Há casos onde $Di=Ti$
$Ri = \sum \left \lceil \frac{Ri}{Tk} \right \rceil Ck + Ci + Bi \leq Di$

> $R1^{0} = C1 + B1 \leq D1$
> $R2^{0} = C1 + C2 + B2 \leq D2$
> $R2^{1} = \left \lceil \frac{R2^{0}}{T1} \right \rceil C1 + C2 + B2 \leq D2$
> $R3^{0} = C1 + C2 + C3 + B3 \leq D3$
> $R3^{1} = \left \lceil \frac{R3^{0}}{T1} \right \rceil C1 + \left \lceil \frac{R3^{0}}{T2} \right \rceil C2 + C3 + B3 \leq D3$
> $R3^{2} = \left \lceil \frac{R3^{1}}{T1} \right \rceil C1 + \left \lceil \frac{R3^{1}}{T2} \right \rceil C2 + C3 + B3 \leq D3$

# Multiprocessadores
O escalonamento de tempo real para multicores ainda não é muito utilizado pois, o barramento e a memória  principal são compartilhados entre os processadores. A solução é isolar todos os elementos compartilhados, mas isso exige um alto grau de complexidade. Atualmente o isolamento é feito via software pelo sistema operacional, mas é esperado que no futuro isso seja aplicado pelo hardware.

## Particionado
+ Costuma ser o mais usual devido a maior previsibilidade. Cada processador escalona as tarefas como um uniprocessador.
+ Cada Processador tem seu próprio algoritmo de escalonamento (P-RM, P-DM, P-EDF)
+ Cada processador possui uma fila própria
+ Teste de escalonabilidade individual
+ Não há migração entre os processadores
+ *Bin Packing*
  + First-Fit: Atribuir a tarefa ao primeiro processador com espaço livre. 
  + Best-Fi: Atribuir a tarefa aos processadores que tem o menor espaço de utilização sobrando
  + Worst-fit: Atribuir a tarefa ao processador que tem o maior espaço de utilização sobrando
    + espalhar as tarefas torna os processadores mais resistentes a sobrecargas
    + melhor manutenção da capacidade inativa para lidar com RTOS
+ *Decreasing*: Alocar tarefas por ordem decrescente de utilização melhora a performance do sistema evitando que processos com maior utilização fiquem de fora.

plain
╔══════════════════════════════════════╗
║             Particionado             ║
╠═════════════════════╦════════════════╣
║      Em projeto     ║    Execução    ║
╠═════════╦═══════════╬════════════════╣
║ Tarefas ║ Algoritmo ║      Filas     ║
╠═════════╬═══════════╬════════════════╣
║         ║     FF    ║ [P-RM]  t5 t2  ║
║  t1 t2  ╠═══════════╬════════════════╣
║  t3 t4  ║     BF    ║ [P-DM]  t1 t4  ║
║  t5 t6  ╠═══════════╬════════════════╣
║         ║     WF    ║ [P-EDF] t3 t6  ║
╚═════════╩═══════════╩════════════════╝


## Global

+ Fila única com um algoritmo de escalonamento (G-RM, G-DM, G-EDF)
+ Troca a tarefa de menor prioridade para executar uma de maior
+ Migração: Uma tarefa de menor prioridade preemptada pode migrar para outro processador
+ A Migração causa atrasos em relação a afinidade na cache

plain
╔══════════════════════════════════════╗
║                Global                ║
╠═════════════════════╦════════════════╣
║      Em projeto     ║    Execução    ║
╠═════════╦═══════════╬════════════════╣
║ Tarefas ║ Algoritmo ║   Fila única   ║
╠═════════╬═══════════╬════════════════╣
║         ║   G-RM    ║                ║
║  t1 t2  ╠═══════════╣                ║
║  t3 t4  ║   G-DM    ║ [G-RM] t1...t6 ║
║  t5 t6  ╠═══════════╣                ║
║         ║   G-EDF   ║                ║
╚═════════╩═══════════╩════════════════╝


## Cluster

+ Utiliza algoritmos Bin Packing para distribuir as tarefas entre os escalonadores
+ Cada escalonador possui um algoritmo Global

plain
╔══════════════════════════════════════════════════════╗
║                  Agrupado (Cluster)                  ║
╠═════════════════════════════════════╦════════════════╣
║              Em projeto             ║    Execução    ║
╠═════════╦═════════════╦═════════════╬════════════════╣
║ Tarefas ║ Algoritmo 1 ║ Algoritmo 2 ║      Filas     ║
╠═════════╬═════════════╬═════════════╬════════════════╣
║         ║      FF     ║    G-RM     ║ [G-RM]  t5 t2  ║
║  t1 t2  ╠═════════════╬═════════════╬════════════════╣
║  t3 t4  ║      BF     ║    G-DM     ║ [G-DM]  t1 t4  ║
║  t5 t6  ╠═════════════╬═════════════╬════════════════╣
║         ║      WF     ║    G-EDF    ║ [G-EDF] t3 t6  ║
╚═════════╩═════════════╩═════════════╩════════════════╝


---

*Copyright &copy; 2019, Paulo Victor Duarte, All rights reserved.*