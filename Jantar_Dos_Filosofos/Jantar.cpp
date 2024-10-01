#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <semaphore.h>  // Inclusão para semáforos

using namespace std;

const int N = 5;  // Número de filósofos e garfos
const int TEMPO_EXECUCAO = 5;

enum Estado { PENSANDO, FOME, COMENDO };
enum Garfo { LIVRE, OCUPADO };


sem_t garfo_semaforos[N];  // Semáforos para cada garfo
mutex cout_mutex;          // Mutex para proteger a saída no console
mutex estado_mutex;        // Mutex para proteger o estado dos filósofos e garfos

void mostrarEstado(Estado filosofos[], Garfo garfos[]) {
    lock_guard<mutex> lock(cout_mutex);  // Para garantir a saída certa, sem alterações
    for (int i = 0; i < N; ++i) {
        cout << (filosofos[i] == PENSANDO ? 'P' : (filosofos[i] == FOME ? 'F' : 'C')) << ",";
    }
    cout << " - ";
    for (int i = 0; i < N; ++i) {
        cout << (garfos[i] == LIVRE ? 'L' : 'O') << ",";
    }
    cout << endl;
}

void filosofosFunc(int id, Estado filosofos[], Garfo garfos[], atomic<bool> &continuar) {
    while (continuar) {
        this_thread::sleep_for(chrono::seconds(1));  // Time pra dar uma refletida na vida (INICIA PENSANDO)
        { //Mudança de estado pra fome
            lock_guard<mutex> lock(estado_mutex);
            filosofos[id] = FOME;
            mostrarEstado(filosofos, garfos);
        }
        this_thread::sleep_for(chrono::seconds(1));  // Tempo de tortura, com fome.

        // Tentar pegar os garfos
        sem_wait(&garfo_semaforos[id]);  // Bloqueia o garfo à esquerda
        sem_wait(&garfo_semaforos[(id + 1) % N]);  // Bloqueia o garfo à direita

        { // Hora do lanche
            lock_guard<mutex> lock(estado_mutex);
            garfos[id] = OCUPADO;  
            garfos[(id + 1) % N] = OCUPADO;  
            filosofos[id] = COMENDO;
            mostrarEstado(filosofos, garfos);
        }
        this_thread::sleep_for(chrono::seconds(1));  // Come ligeiro
        {   //Atualiza estado de liberado para os garfos
            lock_guard<mutex> lock(estado_mutex);
            garfos[id] = LIVRE;             // Atualiza o garfo à esquerda como livre
            garfos[(id + 1) % N] = LIVRE;  // Atualiza o garfo à direita como livre
            filosofos[id] = PENSANDO;
            mostrarEstado(filosofos, garfos);
        }
        // Liberar os garfos
        sem_post(&garfo_semaforos[id]);             // Libera o garfo à esquerda
        sem_post(&garfo_semaforos[(id + 1) % N]);  // Libera o garfo à direita

    }
}

int main() {
    Estado filosofos[N] = { PENSANDO, PENSANDO, PENSANDO, PENSANDO, PENSANDO };
    Garfo garfos[N] = { LIVRE, LIVRE, LIVRE, LIVRE, LIVRE };
    atomic<bool> continuar(true);  //Apenas para condição de parada

    // Inicializa semáforos com valor 1 (livre)
    for (int i = 0; i < N; ++i)
        sem_init(&garfo_semaforos[i], 0, 1);

    thread t[N];
    auto tempo_inicio = chrono::steady_clock::now();

    // Iniciar as threads
    for (int i = 0; i < N; ++i)
        t[i] = thread(filosofosFunc, i, filosofos, garfos, ref(continuar));

    // Verificar o tempo e interrompe as threads após TEMPO_EXECUCAO segundos
    while (true) {
        auto tempo_atual = chrono::steady_clock::now();
        auto duracao = chrono::duration_cast<chrono::seconds>(tempo_atual - tempo_inicio);

        if (duracao.count() >= TEMPO_EXECUCAO) {
            continuar = false;  // Para todas as threads
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(100)); // Pequeno atraso para evitar uso excessivo da CPU (importante)
    }
    
    // Aguarde até que todas as threads terminem
    for (int i = 0; i < N; ++i) {
        t[i].join();
    }

    // Adeus Semaforos
    for (int i = 0; i < N; ++i) {
        sem_destroy(&garfo_semaforos[i]);
    }

    return 0;
}
