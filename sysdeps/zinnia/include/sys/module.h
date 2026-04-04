#ifndef _SYS_MODULE_H
#define _SYS_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

int insertmod(const char *path, const char *cmdline);
int removemod(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MODULE_H */
