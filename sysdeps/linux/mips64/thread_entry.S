
    .abicalls
    .set noreorder

    .section .text
    .global __mlibc_spawn_thread
    .type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
    // __mlibc_spawn_thread(flags, stack, pid_out, child_tid, tls)
    //                         a0,     a1,     a2,        a3,  a4
    //           syscall(NR_clone, flags, stack, ptid, ctid, tls)
    //                         v0,    a0,    a1,   a2,  a4,   a3
    .cpsetup $t9, $t0, __mlibc_spawn_thread

    move $a5, $a4
    move $a4, $a3
    move $a3, $a5

    li $v0, 5055 // NR_clone
    syscall 0

    bne $a3, $zero, error
    nop

    bne $v0, $zero, parent
    nop

    ld $a0, 0($sp)
    ld $a1, 8($sp)
    daddiu $sp, $sp, 16

    dla     $t9, __mlibc_enter_thread
    jalr    $t9
    nop

    break

parent:
    jr $ra
    move $gp, $t0

error:
    negu $v0, $v0
    jr $ra
    move $gp, $t0
