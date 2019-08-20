
#include <errno.h>
#include <qword/memstats.h>

int getmemstats(struct memstats *p) {
    int ret;
    int sys_errno;
    asm volatile (
        "syscall"
        : "=a"(ret), "=d"(sys_errno)
        : "a"(37), "D"(p)
        : "rcx", "r11"
    );
    if (ret == -1) {
        errno = sys_errno;
        return -1;
    }
    return ret;
}
