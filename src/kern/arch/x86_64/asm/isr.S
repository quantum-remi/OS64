section .text
extern isr_handler

%macro PUSH_REGS 0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
%endmacro

%macro POP_REGS 0
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

%macro isr_err_stub 1
global isr_stub_%1
isr_stub_%1:
    push qword %1
    jmp isr_common
%endmacro

%macro isr_no_err_stub 1
global isr_stub_%1
isr_stub_%1:
    push qword 0
    push qword %1
    jmp isr_common
%endmacro

%assign i 0
%rep 32
    %if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17 || i == 21
        isr_err_stub i
    %else
        isr_no_err_stub i
    %endif
%assign i i+1
%endrep

; IRQs (timer, keyboard, etc.)
%assign i 32
%rep 16
    isr_no_err_stub i
%assign i i+1
%endrep

isr_common:
    PUSH_REGS

    mov rdi, [rsp + 15*8] ; vector
    mov rsi, [rsp + 16*8] ; error_code
    mov rdx, [rsp + 17*8] ; rip

    call isr_handler

    POP_REGS

    add rsp, 24 ; pop vector, error_code, rip
    iretq

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 48
    dq isr_stub_%+i
%assign i i+1
%endrep