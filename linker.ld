ENTRY(start)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x7C00;
    .boot : {
        build/bscx.o(.text)
    }

    . = 0x1000;
    .text : {
        build/kernel.o(.text.main)
        build/kernel.o(.text*)
    }
    .data : {
        build/kernel.o(.data*)
    }
    .rodata : {
        build/kernel.o(.rodata*)
    }
    .bss : {
        build/kernel.o(.bss*)
    }
}
