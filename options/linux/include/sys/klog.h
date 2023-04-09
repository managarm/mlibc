#ifndef _SYS_KLOG_H
#define _SYS_KLOG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int klogctl(int type, char *bufp, int len);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_KLOG_H */
