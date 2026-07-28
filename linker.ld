MEMORY
{
    rom (rx)  : ORIGIN = 0x08000000, LENGTH = 0x00080000
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00018000
}
STACK_SIZE = 0x2000;
SECTIONS
{
    _estack = ORIGIN(ram) + LENGTH(ram);
    .text :
    {
        KEEP(*(.vector*))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > rom
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        _edata = .;
        . = ALIGN(4);
    } > ram AT > rom

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
        . = ALIGN(4);
    } > ram
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        . = . + STACK_SIZE;
        . = ALIGN(8);
    } > ram
    _end = .;
}