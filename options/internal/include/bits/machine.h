
#ifndef MLIBC_MACHINE_H
#define MLIBC_MACHINE_H

#if defined (__i386__)
#  define __MLIBC_JMPBUF_SIZE 6
#elif defined (__x86_64__)
#  define __MLIBC_JMPBUF_SIZE 8
#else
#  error "Missing architecture specific code"
#endif

#endif // MLIBC_MACHINE_H

