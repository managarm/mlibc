
#ifndef  _BPF_COMMON_H
#define  _BPF_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

// instruction classes
#define BPF_LD 0x00
#define BPF_ALU 0x04
#define BPF_JMP 0x05
#define BPF_RET 0x06

// ld/ldx fields
#define BPF_W 0x00
#define BPF_ABS 0x20

// alu/jmp fields
#define BPF_AND 0x50

#define BPF_JEQ 0x10
#define BPF_K 0x00

#ifdef __cplusplus
}
#endif __cplusplus

#endif // _BPF_COMMON_H

