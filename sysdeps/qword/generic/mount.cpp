
#include <qword/mount.h>

int mount(const char *source, const char *target,
    const char *filesystemtype, unsigned long mountflags,
    const void *data) {

    int ret;
    register unsigned long arg4_reg asm("r10") = mountflags;
	register const void *arg5_reg asm("r8") = data;
    asm volatile ("syscall" : "=a" (ret)
            : "a"(41), "D"(source), "S"(target),
            "d"(filesystemtype), "r"(arg4_reg), "r"(arg5_reg)
            : "rcx", "r11");
    return ret;
}

int umount(const char *target) {
    int ret;
    asm volatile ("syscall"
                  : "=a" (ret)
                  : "a" (42), "D" (target)
                  : "rcx", "r11");
    return ret;
}
