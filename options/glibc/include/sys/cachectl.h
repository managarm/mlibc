#ifndef _SYS_CACHECTL_H
#define _SYS_CACHECTL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __riscv
int __riscv_flush_icache(void *, void *, unsigned long);
#endif

#ifdef __cplusplus
}
#endif

#endif
