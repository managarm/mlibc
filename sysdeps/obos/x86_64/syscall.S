.intel_syntax noprefix

.global syscall

syscall:
    push rbp
    mov rbp, rsp

    mov eax, edi
    mov rdi, rsi
    mov rsi, rdx
    mov rdx, rcx

    syscall

    leave
    ret

.hidden syscall

.att_syntax prefix

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()