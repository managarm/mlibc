.text
.global _start
.section ".opd", "aw"
.align 8
_start:
    .quad _realstart
    .quad .TOC.
    .quad 0
.text
.global _realstart
.hidden _realstart
_realstart:
    mflr 0
    bl pc
pc:
    mr 14, 1 // store entry stack
    addi 1, 1, -288 // allocate redzone
    std 2, 40(1) // store .TOC.

    mflr 3 // current instruction pointer + 8 because of mflr+bl before it
    addi 3, 3, -8

    addis 4, 2, _start@toc@ha
    ld 4, _start@toc@l(4)
    // lis 4, _realstart@ha
    // addi 4, 4, _realstart@l // r4 = known address at link time
    subf 3, 4, 3 // r3 = _realstart - ip = base load address

    // relocations are not applied yet, the .opd table is not correct
    // so the stubs for calling the .opd entries are not going to work either
    // calling the function manually
    // lis 4, relocateSelfPPC64@ha
    // addi 4, 4, relocateSelfPPC64@l
    addis 4, 2, relocateSelfPPC64@toc@ha
    ld 4, relocateSelfPPC64@toc@l(4)
    add 4, 3, 4
    mtctr 4
    bctrl

    mr 3, 14
    bl interpreterMain
    nop

    stdu 1, -112(1)
    std 2, 40(1)

    ld 2, 8(3) // loading toc and jumping to Scrt1.S entry
    ld 3, 0(3)
    mtctr 3
    bctr

    addi 1, 1, 112
    trap

.section .note.GNU-stack,"",%progbits