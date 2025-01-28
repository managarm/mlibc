#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/debug.hpp>

#include <aero/syscall.h>
#include <sys/types.h>

#define LOG_SIGACTION_INSTALL 0

extern "C" void __mlibc_signal_restore(); // defined in `signals.S`

namespace mlibc {
int sys_sigaction(int how, const struct sigaction *__restrict action,
                  struct sigaction *__restrict old_action) {
#if LOG_SIGACTION_INSTALL
    mlibc::infoLogger() << "sys_sigaction: signal " << how << frg::endlog;
    mlibc::infoLogger() << "sys_sigaction: size: " << sizeof(*action)
                        << frg::endlog;

    if (action != NULL) {
        mlibc::infoLogger() << "sys_sigaction: handler "
                            << (int64_t)action->sa_handler << frg::endlog;
        mlibc::infoLogger() << "sys_sigaction: action "
                            << (int64_t)action->sa_sigaction << frg::endlog;
        mlibc::infoLogger() << "sys_sigaction: flags "
                            << (int64_t)action->sa_flags << frg::endlog;
    }

    mlibc::infoLogger() << frg::endlog;
#endif

    auto sigreturn = (sc_word_t)__mlibc_signal_restore;
    auto ret = syscall(SYS_SIGACTION, how, (sc_word_t)action, sigreturn,
                       (sc_word_t)old_action);

    if(int e = sc_error(ret); e)
        return e;
    return 0;
}

int sys_sigprocmask(int how, const sigset_t *__restrict set,
                    sigset_t *__restrict retrieve) {

    auto ret = syscall(SYS_SIGPROCMASK, how, set, retrieve);
    if(int e = sc_error(ret); e)
		return e;
	return 0;
}
} // namespace mlibc