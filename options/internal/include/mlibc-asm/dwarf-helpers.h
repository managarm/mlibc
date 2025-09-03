#pragma once

// DWARF expressions
#define DW_OP_deref 0x06
#define DW_OP_breg0 0x70

#define DW_CFA_def_cfa_expression 0x0f
#define DW_CFA_expression 0x10

#if defined(__x86_64__)

#define DWARF_REG_RAX 0
#define DWARF_REG_RDX 1
#define DWARF_REG_RCX 2
#define DWARF_REG_RBX 3
#define DWARF_REG_RSI 4
#define DWARF_REG_RDI 5
#define DWARF_REG_RBP 6
#define DWARF_REG_RSP 7
#define DWARF_REG_R8  8
#define DWARF_REG_R9  9
#define DWARF_REG_R10 10
#define DWARF_REG_R11 11
#define DWARF_REG_R12 12
#define DWARF_REG_R13 13
#define DWARF_REG_R14 14
#define DWARF_REG_R15 15
#define DWARF_REG_RETURN_ADDRESS 16

#define DWARF_REG_XMM0 17
#define DWARF_REG_XMM1 18
#define DWARF_REG_XMM2 19
#define DWARF_REG_XMM3 20
#define DWARF_REG_XMM4 21
#define DWARF_REG_XMM5 22
#define DWARF_REG_XMM6 23
#define DWARF_REG_XMM7 24
#define DWARF_REG_XMM8 25
#define DWARF_REG_XMM9 26
#define DWARF_REG_XMM10 27
#define DWARF_REG_XMM11 28
#define DWARF_REG_XMM12 29
#define DWARF_REG_XMM13 30
#define DWARF_REG_XMM14 31
#define DWARF_REG_XMM15 32

#define DWARF_REG_ST0 33
#define DWARF_REG_ST1 34
#define DWARF_REG_ST2 35
#define DWARF_REG_ST3 36
#define DWARF_REG_ST4 37
#define DWARF_REG_ST5 38
#define DWARF_REG_ST6 39
#define DWARF_REG_ST7 40

#define DWARF_REG_MM0 41
#define DWARF_REG_MM1 42
#define DWARF_REG_MM2 43
#define DWARF_REG_MM3 44
#define DWARF_REG_MM4 45
#define DWARF_REG_MM5 46
#define DWARF_REG_MM6 47
#define DWARF_REG_MM7 48

#define DWARF_REG_RFLAGS 49
#define DWARF_REG_ES 50
#define DWARF_REG_CS 51
#define DWARF_REG_SS 52
#define DWARF_REG_DS 53
#define DWARF_REG_FS 54
#define DWARF_REG_GS 55

#define DWARF_REG_TR 62
#define DWARF_REG_LDTR 63
#define DWARF_REG_MXCSR 64
#define DWARF_REG_FCW 65
#define DWARF_REG_FSW 66

#endif // defined(__x86_64__)

#if defined(__ASSEMBLER__)

#define DWARF_ULEB128_14BIT_SIZE(n) (1 + (((n) > 0x7f) & 1))
#define DWARF_SLEB128_14BIT_SIZE(n) (1 + (((n) < -0x40) & 1) + (((n) > 0x3f) & 1))

// write an up to 2-byte signed leb128 value
.macro cfi_emit_sleb128 val
	.if (\val) < -0x2000 || (\val) > 0x1fff // doesn't fit in 2 bytes
		.error "cfi_emit_sleb128 value is out of range (\val)"
	.elseif (\val) < -0x40 || (\val) > 0x3f // doesn't fit in 1 byte
		.cfi_escape ((\val) & 0x7f) | 0x80
		.cfi_escape ((\val) >> 7) & 0x7f
	.else // fits in 1 byte
		.cfi_escape (\val) & 0x7f
	.endif
.endm

// write an up to 2-byte unsigned leb128 value
.macro cfi_emit_uleb128 val
	.if (\val) < 0 || (\val) > 0x3fff // doesn't fit in 2 bytes
		.error "cfi_emit_uleb128 value is out of range (\val)"
	.elseif (\val) > 0x7f // doesn't fit in 1 byte
		.cfi_escape ((\val) & 0x7f) | 0x80
		.cfi_escape (\val) >> 7
	.else
		.cfi_escape (\val)
	.endif
.endm

.macro cfi_set_cfa_to_ptr_with_offset target_reg, offset
	.cfi_escape DW_CFA_def_cfa_expression
	cfi_emit_uleb128 (1 + DWARF_SLEB128_14BIT_SIZE(\offset) + 1)
	.cfi_escape DW_OP_breg0 + (\target_reg)
	cfi_emit_sleb128 (\offset)
	.cfi_escape DW_OP_deref
.endm

// Set previous value of the register 'target_reg' to (context_reg + offset)
.macro cfi_set_prev_reg_value target_reg, context_reg, offset
	.cfi_escape DW_CFA_expression
	cfi_emit_uleb128 (\target_reg)
	cfi_emit_uleb128 (1 + DWARF_SLEB128_14BIT_SIZE(\offset))
	.cfi_escape DW_OP_breg0 + (\context_reg)
	cfi_emit_sleb128 (\offset)
.endm

#endif // defined(__ASSEMBLER__)
