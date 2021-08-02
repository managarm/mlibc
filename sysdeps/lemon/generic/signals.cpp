#include <lemon/syscall.h>

#include <sys/types.h>

#include <mlibc/ansi-sysdeps.hpp>

namespace mlibc{

int sys_sigprocmask(int how, const sigset_t *__restrict set,
	sigset_t *__restrict retrieve){
    int ret = syscall(SYS_SIGPROCMASK, how, set, retrieve);
    if(ret < 0){
        return -ret;
    }

    return 0;
}

int sys_sigaction(int signal, const struct sigaction *__restrict action,
	struct sigaction *__restrict oldAction) {
    int ret = syscall(SYS_SIGNAL_ACTION, signal, action, oldAction);
    if(ret < 0){
        return -ret;
    }

    return 0;
}

int sys_kill(int pid, int signal){
    int ret = syscall(SYS_KILL, pid, signal);
    if(ret < 0){
        return -ret;
    }

    return 0;
}

}