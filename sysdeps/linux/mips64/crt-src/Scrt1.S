    .abicalls
    .set noreorder
    
    .section .text
    .global __start
__start:
    bal 1f
    nop
1:
    # Load gp.
    .cpsetup $ra, $zero, 1b

    move $ra, $zero
    move $fp, $zero

    move $a0, $sp
    dla $a1, main

    dla $t9, __mlibc_entry
    jalr $t9
    nop

    break