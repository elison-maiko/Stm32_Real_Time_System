/****************************************************************************
* MInimal Real-time Operating System (MiROS)
* version 1.26 (matching lesson 26, see https://youtu.be/kLxxXNCrY60)
*
* This software is a teaching aid to illustrate the concepts underlying
* a Real-Time Operating System (RTOS). The main goal of the software is
* simplicity and clear presentation of the concepts, but without dealing
* with various corner cases, portability, or error handling. For these
* reasons, the software is generally NOT intended or recommended for use
* in commercial applications.
*
* Copyright (C) 2018 Miro Samek. All Rights Reserved.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.
*
* Git repo:
* https://github.com/QuantumLeaps/MiROS
****************************************************************************/
#ifndef MIROS_H
#define MIROS_H
#define TICKS_PER_SEC 100U

typedef struct{
	uint32_t cost_abs;
    uint32_t cost_relative;
	uint32_t deadline_abs;
    uint32_t deadline_relative;
    uint32_t period_abs;
    uint32_t period_relative;

} TaskParamets;                          // Definição da estrutura d

/* Thread Control Block (TCB) */
typedef struct {
    void *sp; /* stack pointer */
    uint32_t timeout; /* timeout delay down-counter */
    uint8_t prio; /* thread priority */
    uint8_t index;  //Indice para tratamento
    uint32_t stack_thread [40];
    TaskParamets paramets; /* ... other attributes associated with a thread */
} OSThread;


// -------------- Implementation of Scheduling Rate Monotonic --------------------



int is_schedulable_RTA(TaskParamets tasks[], uint8_t Ntasks);
void OS_redirect_index(void);


// ---------------------------------------------------------------------------------

typedef void (*OSThreadHandler)();

void OS_init(void *stkSto, uint32_t stkSize);

/* callback to handle the idle condition */
void OS_onIdle(void);

/* this function must be called with interrupts DISABLED */
void OS_sched(void);

/* transfer control to the RTOS to run the threads */
void OS_run(void);

/* blocking delay */
void OS_delay(uint32_t ticks);

/* process all timeouts */
void OS_tick(void);

/* callback to configure and start interrupts */
void OS_onStartup(void);

void OSThread_start(
    OSThread *me,
    uint8_t prio, /* thread priority */
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize);

/* ------------------------- SEMAPHORE --------------------------*/
typedef struct{
	uint8_t valor_sem;
} semaphore_t;

void sem_init(semaphore_t *p_sem, uint8_t valor_init); /* Ponteiro p/ semaforo e o valor de inicio*/
void sem_post(semaphore_t *p_sem);
void sem_wait(semaphore_t *p_sem);



#endif /* MIROS_H */
