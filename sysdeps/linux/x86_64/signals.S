#include "mlibc-asm/dwarf-helpers.h"
#include "mlibc-asm/helpers.h"
#include "context-offsets.h"
#include "syscallnos.h"

.section .text
.cfi_startproc
.cfi_signal_frame
	cfi_set_cfa_to_ptr_with_offset DWARF_REG_RSP, UCONTEXT_OFFSET_RSP
	cfi_set_prev_reg_value DWARF_REG_R8, DWARF_REG_RSP, UCONTEXT_OFFSET_R8
	cfi_set_prev_reg_value DWARF_REG_R9, DWARF_REG_RSP, UCONTEXT_OFFSET_R9
	cfi_set_prev_reg_value DWARF_REG_R10, DWARF_REG_RSP, UCONTEXT_OFFSET_R10
	cfi_set_prev_reg_value DWARF_REG_R11, DWARF_REG_RSP, UCONTEXT_OFFSET_R11
	cfi_set_prev_reg_value DWARF_REG_R12, DWARF_REG_RSP, UCONTEXT_OFFSET_R12
	cfi_set_prev_reg_value DWARF_REG_R13, DWARF_REG_RSP, UCONTEXT_OFFSET_R13
	cfi_set_prev_reg_value DWARF_REG_R14, DWARF_REG_RSP, UCONTEXT_OFFSET_R14
	cfi_set_prev_reg_value DWARF_REG_R15, DWARF_REG_RSP, UCONTEXT_OFFSET_R15
	cfi_set_prev_reg_value DWARF_REG_RDI, DWARF_REG_RSP, UCONTEXT_OFFSET_RDI
	cfi_set_prev_reg_value DWARF_REG_RSI, DWARF_REG_RSP, UCONTEXT_OFFSET_RSI
	cfi_set_prev_reg_value DWARF_REG_RBP, DWARF_REG_RSP, UCONTEXT_OFFSET_RBP
	cfi_set_prev_reg_value DWARF_REG_RBX, DWARF_REG_RSP, UCONTEXT_OFFSET_RBX
	cfi_set_prev_reg_value DWARF_REG_RDX, DWARF_REG_RSP, UCONTEXT_OFFSET_RDX
	cfi_set_prev_reg_value DWARF_REG_RAX, DWARF_REG_RSP, UCONTEXT_OFFSET_RAX
	cfi_set_prev_reg_value DWARF_REG_RCX, DWARF_REG_RSP, UCONTEXT_OFFSET_RCX
	cfi_set_prev_reg_value DWARF_REG_RETURN_ADDRESS, DWARF_REG_RSP, UCONTEXT_OFFSET_RIP
	nop

PROC_START_NOCFI(__mlibc_signal_restore)
PROC_ALIAS(__mlibc_signal_restore, __mlibc_signal_restore_rt)
	mov $__NR_rt_sigreturn, %rax
	syscall
	ud2
PROC_END(__mlibc_signal_restore)

GNU_STACK_NOTE()

