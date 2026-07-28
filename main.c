#include <stdint.h>
#include <string.h>

uint32_t task1_stack[256];
uint32_t task2_stack[256];

typedef struct{
    uint32_t* sp;
}TCB;

extern void start_first_task(uint32_t *sp);

// initialize new process with a fake stack, returns sp
uint32_t* initializeTask(uint32_t* stack, void (*func)(void)){
    uint32_t* sp = &stack[256];

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

    return sp;
}

TCB tasks[2];
TCB *current_task;

volatile uint32_t counter1 = 0;
volatile uint32_t counter2 = 0;

void task1_func(void) {while(1){counter1 ++;}}

void task2_func(void) {while(1) {counter2 ++;}}

void systick_init(){

    // enable the clock
    *(volatile uint32_t*)0xE000E010 |= (1 << 0);

    // enable the interrupt
    *(volatile uint32_t*)0xE000E010 |= (1 << 1);

    // use core clock
    *(volatile uint32_t*)0xE000E010 |= (1 << 2);

    // set the reload value
    *(volatile uint32_t*)0xE000E014 = 0xFFFFFFFF;

}

void scheduler(){

    if(current_task == &tasks[0]){
        current_task = &tasks[1];
    } else current_task = &tasks[0];

}

int main(void){

    systick_init();

    tasks[0].sp = initializeTask(task1_stack, task1_func);
    tasks[1].sp = initializeTask(task2_stack, task2_func);

    current_task = &tasks[0];
    
    start_first_task(tasks[0].sp);

    return 0;
}