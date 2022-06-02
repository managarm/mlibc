.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
	sd a7, -8(sp)
	mv a7, a0
	mv a0, a1
	mv a1, a2
	mv a2, a3
	mv a3, a4
	mv a4, a5
	mv a5, a6
	ld a6, -8(sp) // a7
__mlibc_syscall_begin:
	// TODO: cancellation point
	ecall
__mlibc_syscall_end:
	ret // TODO
