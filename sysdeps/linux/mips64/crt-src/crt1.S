    .abicalls
    .set noreorder

    .section .text
    .global __start
__start:
    # Load gp.
    la $gp, _gp

    move $a0, $sp
    la $a1, main
    dla $t9, __mlibc_entry
    jalr $t9
    nop

    break
