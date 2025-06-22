.abiversion 1
.section ".opd", "aw"
.align 8

.global setjmp
.type setjmp, "function"
.global _setjmp
.type _setjmp, "function"
setjmp:
_setjmp:
    .quad .L._setjmp
    .quad .TOC.
    .quad 0

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
    .quad .L.sigsetjmp
    .quad .TOC.
    .quad 0

.global longjmp
.type longjmp, "function"
.global _longjmp
.type _longjmp, "function"
longjmp:
_longjmp:
    .quad .L._longjmp
    .quad .TOC.
    .quad 0

.text

.L.__setjmp:
    // saving gprs
    std 14, 0(3)
    std 15, 8(3)
    std 16, 16(3)
    std 17, 24(3)
    std 18, 32(3)
    std 19, 40(3)
    std 20, 48(3)
    std 21, 56(3)
    std 22, 64(3)
    std 23, 72(3)
    std 24, 80(3)
    std 25, 88(3)
    std 26, 96(3)
    std 27, 104(3)
    std 28, 112(3)
    std 29, 120(3)
    std 30, 128(3)
    std 31, 136(3)

    // saving fprs
    stfd 14, 144(3)
    stfd 15, 152(3)
    stfd 16, 160(3)
    stfd 17, 168(3)
    stfd 18, 176(3)
    stfd 19, 184(3)
    stfd 20, 192(3)
    stfd 21, 200(3)
    stfd 22, 208(3)
    stfd 23, 216(3)
    stfd 24, 224(3)
    stfd 25, 232(3)
    stfd 26, 240(3)
    stfd 27, 248(3)
    stfd 28, 256(3)
    stfd 29, 264(3)
    stfd 30, 272(3)
    stfd 31, 280(3)

    // saving lr
    mflr 14
    std 14, 288(3)

    // saving sp
    std 1, 296(3)
    
    // saving toc
    std 2, 304(3)

    // saving cr
    mfcr 14
    std 14, 312(3)

    cmpwi 5, 0
    bne 1f

    li 3, 0
    blr
1:
    b __sigsetjmp
    nop

.L._setjmp:
    li 5, 0
    b .L.__setjmp

.L.sigsetjmp:
    li 5, 1
    b .L.__setjmp

.L._longjmp:
    // todo(localcc): implement VMX
    // loading gprs
    ld 14, 0(3)
    ld 15, 8(3)
    ld 16, 16(3)
    ld 17, 24(3)
    ld 18, 32(3)
    ld 19, 40(3)
    ld 20, 48(3)
    ld 21, 56(3)
    ld 22, 64(3)
    ld 23, 72(3)
    ld 24, 80(3)
    ld 25, 88(3)
    ld 26, 96(3)
    ld 27, 104(3)
    ld 28, 112(3)
    ld 29, 120(3)
    ld 30, 128(3)
    ld 31, 136(3)

    // loading fprs
    lfd 14, 144(3)
    lfd 15, 152(3)
    lfd 16, 160(3)
    lfd 17, 168(3)
    lfd 18, 176(3)
    lfd 19, 184(3)
    lfd 20, 192(3)
    lfd 21, 200(3)
    lfd 22, 208(3)
    lfd 23, 216(3)
    lfd 24, 224(3)
    lfd 25, 232(3)
    lfd 26, 240(3)
    lfd 27, 248(3)
    lfd 28, 256(3)
    lfd 29, 264(3)
    lfd 30, 272(3)
    lfd 31, 280(3)

    // loading lr
    ld 14, 288(3)
    mtlr 14

    // loading sp
    ld 1, 296(3)

    // loading toc
    ld 2, 304(3)

    // loading cr
    ld 14, 312(3)
    mtcr 14

    mr 3, 4
    cmpwi 3, 0
    bne 1f
    addi 3, 3, 1
1:
    blr

.section .note.GNU-stack,"",%progbits

