LINKER=linker.ld
SRCS=main.c startup.c task_handler.s PendSVHandler.s
CFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -nostdlib -ffreestanding -O0 -Wall -g

all: nucleo.bin

nucleo.elf: $(LINKER) $(SRCS)
	arm-none-eabi-gcc $(CFLAGS) -T $(LINKER) $(SRCS) -o nucleo.elf

nucleo.bin: nucleo.elf
	arm-none-eabi-objcopy -O binary nucleo.elf nucleo.bin

flash: nucleo.bin
	st-flash --reset write nucleo.bin 0x08000000

clean:
	rm -rf nucleo*

