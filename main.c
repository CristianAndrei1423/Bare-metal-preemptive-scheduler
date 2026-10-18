#include "startup.h"

uint32_t task_stack[MAX_TASKS][256];
TCB tasks[MAX_TASKS];
TCB *current_task;
uint32_t task_index;
uint32_t no_tasks;
volatile uint32_t system_ticks;

// initialize new process with a fake stack, returns the id of the task
// uint32_t* initializeTask(uint32_t* stack, void (*func)(void)){
uint32_t initializeTask(void (*func)(void)){
    
    // initialize the stack
    uint32_t* sp = &task_stack[no_tasks][256];

    *(--sp) = (1 << 24); // xPSR, set thumb bit
    *(--sp) = (uint32_t)func & ~1UL; // took into account the thumb bit because of a hardfault
    *(--sp) = 0xdeadbeef; // LR 
    // NOTE : When you want the process to terminate, make this point to a cleanup func ^
    *(--sp) = 12; // R12
    *(--sp) = 3; // R3
    *(--sp) = 2; // R2
    *(--sp) = 1; // R1
    *(--sp) = 0; // R0

    *(--sp) = 11; // R11
    *(--sp) = 10; // R10
    *(--sp) = 9; // R9
    *(--sp) = 8; // R8
    *(--sp) = 7; // R7
    *(--sp) = 6; // R6
    *(--sp) = 5; // R5
    *(--sp) = 4; // R4

    tasks[no_tasks].sp = sp;
    tasks[no_tasks].state = READY;
    tasks[no_tasks].index = no_tasks;
    no_tasks ++;

    return no_tasks - 1;
}

volatile uint32_t counter1 = 0;
volatile uint32_t counter2 = 0;

void idle_func(void) {
    while(1) {__asm__("wfi");};
}

void task1_func(void) {
    while(1){
        counter1 ++;

        if(counter1 % 10000 == 0) sleep_ms(2);
    }
}

void task2_func(void) {
    while(1) {
        counter2 ++;
        
        if(counter2 % 10000 == 0) sleep_ms(2);
    }
}

void systick_init(){

    // enable the clock
    *(volatile uint32_t*)0xE000E010 |= (1 << 0);

    // enable the interrupt
    *(volatile uint32_t*)0xE000E010 |= (1 << 1);

    // use core clock
    *(volatile uint32_t*)0xE000E010 |= (1 << 2);

    // set the reload value
    *(volatile uint32_t*)0xE000E014 = 0x3E7F; // 16 000 - 1 for 1 ms

}

void scheduler(){

    if(current_task->state == RUNNING) current_task->state = READY;

    for(int i = 0; i < no_tasks; i++){
        // lazily check the wake for wake_tick for the next task

        task_index = (task_index + 1) % no_tasks;

        if(tasks[task_index].state == BLOCKED && tasks[task_index].wake_tick <= system_ticks){
            tasks[task_index].state = READY;
        }

        if(tasks[task_index].state == READY && task_index != 0){
            tasks[task_index].state = RUNNING;
            current_task = &tasks[task_index];
            return;
        }
    }

    // there is no task READY, put the idle_task
    current_task = &tasks[0];
    task_index = 0;
    tasks[0].state = RUNNING;

}

void sleep_ms(uint32_t ms){

    uint32_t ind = (*current_task).index;
    tasks[ind].wake_tick = system_ticks + ms;
    tasks[ind].state = BLOCKED;

    *(volatile uint32_t*)0xE000ED04 = (1 << 28); // pend the switch

}

int main(void){

    systick_init();
    initializeTask(idle_func);
    initializeTask(task1_func);
    initializeTask(task2_func);
    
    current_task = &tasks[1];
    current_task->state = RUNNING;
    task_index = 1;

    start_first_task(tasks[1].sp);

    return 0;
}