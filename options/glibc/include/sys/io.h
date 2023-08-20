#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <bits/inline-definition.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int ioperm(unsigned long int from, unsigned long int num, int turn_on);

__attribute__((deprecated)) int iopl(int level);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __x86_64__
__MLIBC_INLINE_DEFINITION unsigned char inb(unsigned short int port) {
	unsigned char _v;
	__asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
	return _v;
}

__MLIBC_INLINE_DEFINITION unsigned char inb_p(unsigned short int port) {
	unsigned char _v;
	__asm__ __volatile__ ("inb %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
	return _v;
}

__MLIBC_INLINE_DEFINITION unsigned short int inw(unsigned short int port) {
	unsigned short _v;
	__asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (port));
	return _v;
}

__MLIBC_INLINE_DEFINITION unsigned short int inw_p(unsigned short int port) {
	unsigned short int _v;
	__asm__ __volatile__ ("inw %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
	return _v;
}

__MLIBC_INLINE_DEFINITION unsigned int inl(unsigned short int port) {
	unsigned int _v;
	__asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (port));
	return _v;
}

__MLIBC_INLINE_DEFINITION unsigned int inl_p(unsigned short int port) {
	unsigned int _v;
	__asm__ __volatile__ ("inl %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (port));
	return _v;
}

__MLIBC_INLINE_DEFINITION void outb(unsigned char value, unsigned short int port) {
	__asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd" (port));
}

__MLIBC_INLINE_DEFINITION void outb_p(unsigned char value, unsigned short int port) {
	__asm__ __volatile__ ("outb %b0,%w1\noutb %%al,$0x80": :"a" (value), "Nd" (port));
}

__MLIBC_INLINE_DEFINITION void outw(unsigned short int value, unsigned short int port) {
	__asm__ __volatile__ ("outw %w0,%w1": :"a" (value), "Nd" (port));
}

__MLIBC_INLINE_DEFINITION void outw_p(unsigned short int value, unsigned short int port) {
	__asm__ __volatile__ ("outw %w0,%w1\noutb %%al,$0x80": :"a" (value), "Nd" (port));
}

__MLIBC_INLINE_DEFINITION void outl(unsigned int value, unsigned short int port) {
	__asm__ __volatile__ ("outl %0,%w1": :"a" (value), "Nd" (port));
}

__MLIBC_INLINE_DEFINITION void outl_p(unsigned int value, unsigned short int port) {
	__asm__ __volatile__ ("outl %0,%w1\noutb %%al,$0x80": :"a" (value), "Nd" (port));
}

__MLIBC_INLINE_DEFINITION void insb(unsigned short int port, void *addr, unsigned long int count) {
	__asm__ __volatile__ ("cld ; rep ; insb":"=D" (addr), "=c" (count) :"d" (port), "0" (addr), "1" (count));
}

__MLIBC_INLINE_DEFINITION void insw(unsigned short int port, void *addr, unsigned long int count) {
	__asm__ __volatile__ ("cld ; rep ; insw":"=D" (addr), "=c" (count) :"d" (port), "0" (addr), "1" (count));
}

__MLIBC_INLINE_DEFINITION void insl(unsigned short int port, void *addr, unsigned long int count) {
	__asm__ __volatile__ ("cld ; rep ; insl":"=D" (addr), "=c" (count) :"d" (port), "0" (addr), "1" (count));
}

__MLIBC_INLINE_DEFINITION void outsb(unsigned short int port, const void *addr, unsigned long int count) {
	__asm__ __volatile__ ("cld ; rep ; outsb":"=S" (addr), "=c" (count) :"d" (port), "0" (addr), "1" (count));
}

__MLIBC_INLINE_DEFINITION void outsw(unsigned short int port, const void *addr, unsigned long int count) {
	__asm__ __volatile__ ("cld ; rep ; outsw":"=S" (addr), "=c" (count) :"d" (port), "0" (addr), "1" (count));
}

__MLIBC_INLINE_DEFINITION void outsl(unsigned short int port, const void *addr, unsigned long int count) {
	__asm__ __volatile__ ("cld ; rep ; outsl":"=S" (addr), "=c" (count) :"d" (port), "0" (addr), "1" (count));
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IO_H */
