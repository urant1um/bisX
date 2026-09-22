[BITS 32]
[GLOBAL _start]
[EXTERN bisX_main]

_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    call bisX_main

.loop:
    hlt
    jmp .loop
