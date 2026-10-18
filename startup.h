#include <stdint.h>
#include <string.h>

extern uint32_t _estack; // top of stack, defined in the linker script
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss, _ebss;
extern int main(void);
extern void SVCHandler(void);
extern void PendSV(void);

// vector table lives at the very start of flash
typedef void (*vector_func)(void);

#define MAX_TASKS 10

typedef enum {READY, RUNNING, BLOCKED} TaskState;

typedef struct{
    uint32_t* sp;
    TaskState state;
    uint32_t wake_tick;
    uint32_t index;
}TCB;

extern uint32_t task_stack[MAX_TASKS][256];
extern TCB tasks[MAX_TASKS];
extern TCB *current_task;
extern uint32_t task_index;
extern uint32_t no_tasks;
extern volatile uint32_t system_ticks;


extern void start_first_task(uint32_t *sp);
void sleep_ms(uint32_t ms);
