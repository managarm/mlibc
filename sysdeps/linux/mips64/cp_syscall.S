.abicalls
.set noreorder

.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
    move $v0, $a0
    move $a0, $a1
    move $a1, $a2
    move $a2, $a3
    move $a3, $a4
    move $a4, $a5
    move $a5, $a6
    move $a6, $a7
    rdhwr $v1, $29
    lw $t0, -0x7068($v1)

__mlibc_syscall_begin:
    li $t1, (1 << 0) | (1 << 2)
    and $t0, $t0, $t1
    beq $t0, $t1, cancel
    nop
    syscall
__mlibc_syscall_end:
    bne $a3, $zero, fail
    nop

    jr $ra
    nop

fail:
    neg $v0, $v0
    jr $ra
    nop

cancel:
    .cpsetup $t9, $t0, __mlibc_do_asm_cp_syscall
    dla $t9, __mlibc_do_cancel
    jalr $t9
    nop
    break
.section .note.GNU-stack,"",%progbits