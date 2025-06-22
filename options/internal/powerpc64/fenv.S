.abiversion 1
.section ".opd", "aw"
.align 8

.global feclearexcept
.type feclearexcept,@function
feclearexcept:
    .quad .L.feclearexcept
    .quad .TOC.
    .quad 0

.global feraiseexcept
.type feraiseexcept,@function
feraiseexcept:
    .quad .L.feraiseexcept
    .quad .TOC.
    .quad 0

.global __fesetround
.hidden __fesetround
.type __fesetround,@function
__fesetround:
    .quad .L.__fesetround
    .quad .TOC.
    .quad 0

.global fegetround
.type fegetround,@function
fegetround:
    .quad .L.fegetround
    .quad .TOC.
    .quad 0

.global fegetenv
.type fegetenv,@function
fegetenv:
    .quad .L.fegetenv
    .quad .TOC.
    .quad 0

.global fesetenv
.type fesetenv,@function
fesetenv:
    .quad .L.fesetenv
    .quad .TOC.
    .quad 0

.global fetestexcept
.type fetestexcept,@function
fetestexcept:
    .quad .L.fetestexcept
    .quad .TOC.
    .quad 0

.text 

.L.feclearexcept:
    addi 1, 1, -8

    mffs 4
    stfd 4, 0(1)

    ld 4, 0(1)

    nor 3, 3, 3
    and 4, 3, 4

    std 4, 0(1)
    lfd 4, 0(1)
    mtfsf 0xff, 4

    addi 1, 1, 8
    blr

.L.feraiseexcept:
    addi 1, 1, -8
    
    mffs 4
    stfd 4, 0(1)

    ld 4, 0(1)

    or 4, 3, 4

    std 4, 0(1)
    lfd 4, 0(1)
    mtfsf 0xff, 4

    li 3, 0

    addi 1, 1, 8
    blr

.L.__fesetround:
    addi 1, 1, -8

    mffs 4
    stfd 4, 0(1)
    ld 4, 0(1)

    rlwinm 4, 4, 0, 0, 29
    or 4, 3, 4

    std 4, 0(1)
    lfd 4, 0(1)
    mtfsf 0xff, 4

    addi 1, 1, 8
    blr

.L.fegetround:
    addi 1, 1, -8

    mffs 4
    stfd 4, 0(1)
    ld 4, 0(1)
    
    andi. 3, 4, 0b11

    addi 1, 1, 8
    blr

.L.fegetenv:
    addi 1, 1, -8

    mffs 4
    stfd 4, 0(1)
    ld 4, 0(1)

    rldicr 4, 4, 32, 31
    std 4, 0(3)

    li 3, 0

    addi 1, 1, 8
    blr

.L.fesetenv:
    addi 1, 1, -8

    ld 4, 0(3)
    srdi 4, 4, 32

    std 4, 0(1)
    lfd 4, 0(1)
    mtfsf 0xff, 4

    li 3, 0

    addi 1, 1, 8
    blr

.L.fetestexcept:
    addi 1, 1, -8

    mffs 4
    stfd 4, 0(1)
    ld 4, 0(1)

    and 3, 4, 3

    addi 1, 1, 8
    blr

.section .note.GNU-stack,"",%progbits

