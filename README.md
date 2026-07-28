# Bare-metal preemptive scheduler for ARM-CORTEX M4

## Quick Overview

### What :

This project features the first step of an RTOS, the preemptive
scheduler developed for the ARM CORTEX-M4 CPU. The hardware used
for this project is the [NUCLEO-F401RE](https://www.st.com/en/evaluation-tools/nucleo-f401re.html)
board by STMicroelectronics.

### Why :

This project aims to be one of the first steps in exploring the
world of embedded systems and to deepen my understanding of the
CPU and its niche functionalities.

## Usage

Note : to upload the resulting binary, you should use a board with the afore-mentioned CPU.

### Prerequisites

- Compiler / Linker / Assembler

```bash
sudo apt install gcc-arm-none-eabi
```

- On-chip access

```bash
sudo apt install stlink-tools
```

- Optional : Debugger

```bash
sudo apt install gdb-multiarch
```

### Make commands

- `make` to build the .elf

- `make flash` to dump the binary into the board's flash

- `make clean` to remove the built files

## Functionality

### The boot chain

The linker script is the `linker.ld` file, it has the purpose of 
telling the compiler where to put each memory section (bss, data
etc.), how much memory the flash and RAM have and from what 
virtual address they start from. It is also used to align these
sections to 4 and 8 bytes depending on the documentation. In this
file I also created some variables that help me initialize the
memory sections in the following steps.

The `startup.c` file contains the **IVT** (interrupt vector
table). This vector is put at the start of the flash memory with
the help of the linker script. The IVT's role is to store both
the initial stack pointer the CPU loads at startup and the
**Exception Handlers** .The ones used in this project are the
ResetHandler, the SVCHandler, PendSV and SystickHandler, the
others are just placeholders. The `ResetHandler` function
initializes the data memory region with the values of the
variables after .text, zero's out .bss and calls main.

### The context switch

A context switch consists of several parts:

- Save the context of the paused program on its stack (its registers)
- change the stack pointer to the starting task's stack pointer
- reinstate the starting program's context

In this project this is done with the help of exceptions. The
SysTick handler fires upon reaching 0, pending PendSV and the
scheduler selects the next task to run (round-robin selection).
The PendSV does the context switch and the task is resumed.

The registers we need to save manually are R4-R11, as the other
registers are saved by the CPU automatically.

At startup, the main program uses the **MSP** (main stack
pointer) register, but for running multiple tasks we need to use
the **PSP** (process stack pointer). To tell the CPU which one to
use we make use of the **LR** (link register) magic value :
`EXC_RETURN`. By manipulating this value only in handler mode (in
which we enter in the `start_first_task` function by `svc #0` or
when SysTick makes an exception), we can tell the CPU in which
mode to run and the stack pointer to use, as implemented in the
`SVCHandler` and `PendSV` functions.

At startup, when first entering a task, there is no context to
load from, so we need to make an artificial context
(`initializeTask` function). Additionally, since we use different
stack pointers, we don't want to save the context of the main
function on its stack, so a special function needs to be called
that only loads the first task's artificial context: `SVCHandler`.

The context switch frequency is determined by the reload value
set in the `systick_init` function. 

## Acknowledgments

### Sources

- The [memfault blog series](https://interrupt.memfault.com/)
introduction to embedded systems.

- The RM0368 reference manual

- The STM32F401xE datasheet

- The UM1724 user manual

- Daniele Lacamera "Embedded Systems Architecture"

- Joseph Yiu "The definitive guide to ARM CORTEX-M3 and ARM
CORTEX-M4 processors"

### AI USE

**All of the code in this project is written by me**.
I prompted the Claude Opus 4.8 Model to act like a mentor so that
it wouldn't give me the answer but rather give me a direction in
which to go. Nothing was copy-pasted from the AI. It let me
figure things on my own and when I got stuck, nudge me in the
right direction. It explained the hard parts and told me in which
chapter / section to look into for documentation (the books were
also recommended by Claude). It also made me debug the errors
myself using gdb and the server made by the `st-util` command.
The sections where I took its advice were the `Makefile` and
`linker.ld`,in that it told me a better, simpler way to implement
the required functionality.