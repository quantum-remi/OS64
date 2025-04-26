section .text
global gdt_load

gdt_load:
    lgdt [rdi]
    push 0x08         ; CS selector (kernel code)
    lea rax, [rel .reload_cs]
    push rax
    retfq             ; Far return to reload CS
.reload_cs:
    mov ax, 0x10      ; DS/ES/SS selector (kernel data)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
