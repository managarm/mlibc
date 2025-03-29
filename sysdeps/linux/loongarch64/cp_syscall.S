.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
	st.d $a7, $sp, -8
	move $a7, $a0
	move $a0, $a1
	move $a1, $a2
	move $a2, $a3
	move $a3, $a4
	move $a4, $a5
	move $a5, $a6
	move $a6, $a7
	ld.w $t0, $tp, -96 // Tcb::cancelBits. See asserts in tcb.hpp.
__mlibc_syscall_begin:
	// tcbCancelEnableBit && tcbCancelTriggerBit
	addi.d $t1, $r0, (1 << 0) | (1 << 2)
	and $t0, $t0, $t1
	beq $t0, $t1, cancel
	syscall 0
__mlibc_syscall_end:
	ret

cancel:
	la.local $t2, __mlibc_do_cancel
	jr $t2
.section .note.GNU-stack,"",%progbits

