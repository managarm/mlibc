.section .init
.global _init
_init:
    addi.d $sp, $sp, -16
    st.d $ra, $sp, 8

.section .fini
.global _fini
_fini:
    addi.d $sp, $sp, -16
    st.d $ra, $sp, 8

.section .note.GNU-stack,"",%progbits
