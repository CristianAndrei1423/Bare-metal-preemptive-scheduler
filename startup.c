#include "startup.h"

void ResetHandler(void) {

    uint32_t *start_t = &_etext;
    uint32_t *start_d = &_sdata;

    if(start_d != start_t){
        while(start_d < &_edata){
            *start_d++ = *start_t++;
        }
    }

    for(uint32_t* i = &_sbss; i < &_ebss; i++){
        *i = 0;
    }

    main();

    while(1);
}

void Default_Handler(void) { while (1); }

void SysTickHandler(void) {
    *(volatile uint32_t*)0xE000ED04 = (1 << 28);
    system_ticks++;
}

__attribute__((section(".vector"), used))
const vector_func vector_table[] = {
    (vector_func)&_estack,   // entry 0: initial stack pointer
    ResetHandler,            // entry 1: reset -> your code
    Default_Handler,         // 2: NMI
    Default_Handler,         // 3: HardFault
    Default_Handler,
    Default_Handler,
    Default_Handler,
    0,
    0,
    0,
    0,
    SVCHandler,
    Default_Handler,
    0,
    PendSV,
    SysTickHandler,
};