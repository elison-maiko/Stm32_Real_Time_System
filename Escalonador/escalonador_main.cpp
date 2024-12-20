#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "miros.h"

uint32_t stack_idleThread[40];

uint8_t current_task = 0;


void task1(){
	while(1){
		current_task = 1;
	}
}
void task2(){
	while(1){
		current_task = 2;
		}
}
void task3(){
	while(1){
		current_task = 3;
		}
}

OSThread thread_task1;
OSThread thread_task2;
OSThread thread_task3;

int main() {

    OS_init(stack_idleThread, sizeof(stack_idleThread));

	thread_task1.paramets.period_abs = 600;
	thread_task1.paramets.period_relative =600;
	thread_task1.paramets.cost_abs = 200;
	thread_task1.paramets.cost_relative = 200;

	thread_task2.paramets.period_abs = 800;
	thread_task2.paramets.period_relative =800;
	thread_task2.paramets.cost_abs = 200;
	thread_task2.paramets.cost_relative = 200;

	thread_task3.paramets.period_abs = 1000;
	thread_task3.paramets.period_relative =1000;
	thread_task3.paramets.cost_abs = 200;
	thread_task3.paramets.cost_relative = 200;

	OSThread_start(&thread_task1, 1, &task1, thread_task1.stack_thread, sizeof(thread_task1.stack_thread));
	OSThread_start(&thread_task2, 2, &task2, thread_task2.stack_thread, sizeof(thread_task2.stack_thread));
	OSThread_start(&thread_task3, 3, &task3, thread_task3.stack_thread, sizeof(thread_task3.stack_thread));

    /* transfer control to the RTOS to run the threads */
    OS_run();

    //return 0;
}
