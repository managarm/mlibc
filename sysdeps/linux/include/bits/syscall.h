#ifndef _MLIBC_SYSCALL_H
#define _MLIBC_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef long __sc_word_t;

// These syscall macros were copied from musl.
#define __scc(x) ((__sc_word_t)(x))
#define __syscall0(n) __do_syscall0(n)
#define __syscall1(n,a) __do_syscall1(n,__scc(a))
#define __syscall2(n,a,b) __do_syscall2(n,__scc(a),__scc(b))
#define __syscall3(n,a,b,c) __do_syscall3(n,__scc(a),__scc(b),__scc(c))
#define __syscall4(n,a,b,c,d) __do_syscall4(n,__scc(a),__scc(b),__scc(c),__scc(d))
#define __syscall5(n,a,b,c,d,e) __do_syscall5(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e))
#define __syscall6(n,a,b,c,d,e,f) __do_syscall6(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f))
#define __syscall7(n,a,b,c,d,e,f,g) __do_syscall7(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f),__scc(g))
#define __SYSCALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_X(__VA_ARGS__,7,6,5,4,3,2,1,0,)
#define __SYSCALL_CONCAT_X(a,b) a##b
#define __SYSCALL_CONCAT(a,b) __SYSCALL_CONCAT_X(a,b)
#define __SYSCALL_DISP(b,...) __SYSCALL_CONCAT(b,__SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)
#define __syscall(...) __SYSCALL_DISP(__syscall,__VA_ARGS__)
#define syscall(...) __do_syscall_ret(__syscall(__VA_ARGS__))

// These functions are implemented in arch-syscall.cpp.
__sc_word_t __do_syscall0(long);
__sc_word_t __do_syscall1(long, __sc_word_t);
__sc_word_t __do_syscall2(long, __sc_word_t, __sc_word_t);
__sc_word_t __do_syscall3(long, __sc_word_t, __sc_word_t, __sc_word_t);
__sc_word_t __do_syscall4(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t);
__sc_word_t __do_syscall5(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t);
__sc_word_t __do_syscall6(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t);
__sc_word_t __do_syscall7(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t, __sc_word_t);
long __do_syscall_ret(unsigned long);

#ifdef __cplusplus
}
#endif

#endif // _MLIBC_SYSCALL_H
