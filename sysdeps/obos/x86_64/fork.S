.intel_syntax noprefix

#define Sys_ThreadContextCreateFork 0x80000001
#define Sys_HandleClone 0x5
#define Sys_HandleClose 0x4
#define Sys_ProcessStart 0x14
#define Sys_ThreadCreate 0x9
#define Sys_ProcessGetPid 0x3A
#define Sys_MmFork 0x3D

#define HANDLE_CURRENT 0xfe000000

#define THREAD_PRIORITY_NORMAL 2

// rdi: input, pid_t* child
// rbx: local, handle vmm_context
// r12: local, handle thr_context
// r13: local, handle thread_handle
// r14: local, handle proc_handle

obos_fork:
    push rbp
    mov rbp, rsp

    push rbx
    push r12
    push r13
    push r14
    push r15

    // Set *child to zero
    mov DWORD PTR [rdi], 0

    push rdi

    // Fork the current VMM context
    mov eax, Sys_MmFork
    syscall
    mov rbx, rax

    // Make the thread context
    lea rsi, [rsp+0x8] // after the saved RDI register
    mov eax, Sys_ThreadContextCreateFork
    lea rdi, [rip+fork_shim]
    mov rdx, rbx
    syscall
    mov r12, rax

    // Create a thread
    mov eax, Sys_ThreadCreate
    mov rdi, THREAD_PRIORITY_NORMAL
    xor rsi,rsi
    xor rdx,rdx
    mov r8, r12
    syscall
    mov r13, rax

    mov rdi, rax

    // Start the process
    mov eax, Sys_ProcessStart
    mov rsi, rbx
    mov rdx, 1
    syscall
    mov r14, rax

    // Get the PID
    // TODO: Can this be a race condition if the process ends fast enough?
    mov rdi, r14
    mov eax, Sys_ProcessGetPid
    syscall

    pop rdi

    mov [rdi], eax

    mov rdi, r14
    mov eax, Sys_HandleClose
    syscall

    mov rdi, r13
    mov eax, Sys_HandleClose
    syscall

    mov rdi, r12
    mov eax, Sys_HandleClose
    syscall

    mov rdi, rbx
    mov eax, Sys_HandleClose
    syscall

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    leave
    xor rax,rax
    ret

fork_shim:

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    xor rax,rax

    pop rbp
    ret

.global obos_fork
.hidden obos_fork

.att_syntax prefix

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
