    .abicalls
    .set noreorder

#if defined(__mips_hard_float)
    .section .text

    .global feclearexcept
    .type feclearexcept, %function
feclearexcept:
    cfc1 $t1, $31
    andi $a0, $a0, 0x7c
    sll $t0, $a0, 10
    or $t0, $a0, $t0
    nor $t0, $t0, $zero
    and $t0, $t1, $t0
    move $v0, $zero
    jr $ra
    ctc1 $t0, $31

    .global feraiseexcept
    .type feraiseexcept, %function
feraiseexcept:
    cfc1 $t0, $31
    andi $a0, $a0, 0x7c
    sll $t1, $a0, 10
    or $t0, $t0, $t1
    or $t0, $t0, $a0
    move $v0, $zero
    jr $ra
    ctc1 $t0, $31

    .global fetestexcept
    .type fetestexcept, %function
fetestexcept:
    cfc1 $v0, $31
    andi $a0, $a0, 0x7c
    jr $ra
    and $v0, $a0, $v0

    .global fegetround
    .type fegetround, %function
fegetround:
    cfc1 $v0, $31
    jr $ra
    andi $v0, $v0, 3

    .global __fesetround
    .type __fesetround, %function
__fesetround:
    cfc1 $t0, $31
    li $t1, 0xFFFFFFFC
    and $t0, $t0, $t1
    or $t0, $t0, $a0
    move $v0, $zero
    jr $ra
    ctc1 $t0, $31

    .global fegetenv
    .type fegetenv, %function
fegetenv:
    cfc1 $t0, $31
    move $v0, $zero
    jr $ra
    sw $t0, 0($a0)

    .global fesetenv
    .type fesetenv, %function
fesetenv:
    li $t1, -1
    beq $a0, $t1, 1f
#ifdef __mips_nan2008
    lui $t0, 0x000c
#else
    move $t0, $zero
#endif
    lw $t0, 0($a0)
1:
    move $v0, $zero
    jr $ra
    ctc1 $t0, $31

#endif
