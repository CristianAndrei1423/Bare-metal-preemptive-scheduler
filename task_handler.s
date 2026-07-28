.syntax unified
.cpu cortex-m4
.thumb

.global start_first_task
.type start_first_task, %function

.global SVCHandler
.type SVCHandler, %function

start_first_task:
    
    // load current task sp 
    ldr r1, =current_task
    ldr r1, [r1]
    str r0, [r1]
    
    svc #0


SVCHandler:

    // the address sp is still in r0
    // load the registers from our side

    ldr r1, =current_task
    ldr r1, [r1]
    ldr r0, [r1]

    ldmia r0!, {r4-r11}

    // now because ! means advance, r0 changed,
    // and is the new correct value of psp

    msr psp, r0

    // put the EXC_RETURN value in LR

    ldr lr, =0xFFFFFFFD

    bx lr