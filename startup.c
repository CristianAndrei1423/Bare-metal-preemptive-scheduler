#include <stdint.h>

extern uint32_t _estack; // top of stack, defined in the linker script
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss, _ebss;
extern int main(void);
extern void SVCHandler(void);
extern void PendSV(void);

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

// vector table lives at the very start of flash
typedef void (*vector_func)(void);

void Default_Handler(void) { while (1); }

void SysTickHandler(void) {
    *(volatile uint32_t*)0xE000ED04 = (1 << 28);
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