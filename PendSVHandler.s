.syntax unified
.cpu cortex-m4
.thumb

.global PendSV
.type PendSV, %function

.extern scheduler

PendSV:

    // first save everything

    // load psp
    mrs r0, psp

    // save the registers to the stack
    stmdb r0!, {r4-r11}

    // store the sp into the current_task sp
    ldr r1, =current_task
    ldr r1, [r1]
    str r0, [r1]

    // preserve lr when calling the scheduler
    push {lr}

    bl scheduler

    pop {lr}

    // load the new sp for the new task
    
    ldr r1, =current_task
    ldr r1, [r1]
    ldr r0, [r1]

    // restore all registers from sp
    ldmia r0!, {r4-r11}

    // put psp to current sp
    msr psp, r0

    // set lr to thread mode and use psp
    ldr lr, =0xFFFFFFFD

    bx lr