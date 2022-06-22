#ifndef _SYS_KLOG_H
#define _SYS_KLOG_H

#ifdef __cplusplus
extern "C" {
#endif

int klogctl(int type, char *bufp, int len);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_KLOG_H */
