#include "mlibc-asm/helpers.h"
#include "mlibc-asm/jmp_buf.h"

    .abicalls
    .set noreorder

    .hidden __setjmp
    .type __setjmp, "function"
__setjmp:
    sd $s0, 0($a0)
    sd $s1, 8($a0)
    sd $s2, 16($a0)
    sd $s3, 24($a0)
    sd $s4, 32($a0)
    sd $s5, 40($a0)
    sd $s6, 48($a0)
    sd $s7, 56($a0)
    sd $gp, 64($a0)
    sd $sp, 72($a0)
    sd $s8, 80($a0)

#if defined(__mips_hard_float)
    sdc1 $f24, 96($a0)
    sdc1 $f25, 104($a0)
    sdc1 $f26, 112($a0)
    sdc1 $f27, 120($a0)
    sdc1 $f28, 128($a0)
    sdc1 $f29, 136($a0)
    sdc1 $f30, 144($a0)
    sdc1 $f31, 152($a0)
#endif
    bne $a2, $zero, 1f
    sd $ra, 88($a0)
    sw $zero, JMP_BUF_SAVESIGS_OFFSET($a0)
    jr $ra
    move $v0, $zero

1:
    .cpsetup $t9, $t0, sigsetjmp
    dla $t9, __sigsetjmp
    j $t9
    move $gp, $t0

    .global setjmp
    .type setjmp, "function"
    .global _setjmp
    .type _setjmp, "function"
setjmp:
_setjmp:
    move $a1, $zero
    move $a2, $zero
	j __setjmp
    nop

    .global sigsetjmp
    .type sigsetjmp, "function"
sigsetjmp:
    li $a2, 1
    j __setjmp
    nop

    .global longjmp
    .type longjmp, "function"
    .global _longjmp
    .type _longjmp, "function"
longjmp:
_longjmp:
    ld $s0, 0($a0)
    ld $s1, 8($a0)
    ld $s2, 16($a0)
    ld $s3, 24($a0)
    ld $s4, 32($a0)
    ld $s5, 40($a0)
    ld $s6, 48($a0)
    ld $s7, 56($a0)
    ld $gp, 64($a0)
    ld $sp, 72($a0)
    ld $s8, 80($a0)

#if defined(__mips_hard_float)
    ldc1 $f24, 96($a0)
    ldc1 $f25, 104($a0)
    ldc1 $f26, 112($a0)
    ldc1 $f27, 120($a0)
    ldc1 $f28, 128($a0)
    ldc1 $f29, 136($a0)
    ldc1 $f30, 144($a0)
    ldc1 $f31, 152($a0)
#endif
    ld $ra, 88($a0)

    sltiu $v0, $a1, 1
    jr $ra
    add $v0, $v0, $a1

GNU_STACK_NOTE()