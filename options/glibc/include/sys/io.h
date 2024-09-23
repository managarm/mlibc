#ifndef _SYS_IO_H
#define _SYS_IO_H

#include <bits/inline-definition.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int ioperm(unsigned long int __from, unsigned long int __num, int __turn_on);

__attribute__((deprecated)) int iopl(int __level);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __x86_64__
__MLIBC_INLINE_DEFINITION unsigned char inb(unsigned short int __port) {
	unsigned char __value;
	__asm__ __volatile__ ("inb %w1,%0":"=a" (__value):"Nd" (__port));
	return __value;
}

__MLIBC_INLINE_DEFINITION unsigned char inb_p(unsigned short int __port) {
	unsigned char __value;
	__asm__ __volatile__ ("inb %w1,%0\noutb %%al,$0x80":"=a" (__value):"Nd" (__port));
	return __value;
}

__MLIBC_INLINE_DEFINITION unsigned short int inw(unsigned short int __port) {
	unsigned short __value;
	__asm__ __volatile__ ("inw %w1,%0":"=a" (__value):"Nd" (__port));
	return __value;
}

__MLIBC_INLINE_DEFINITION unsigned short int inw_p(unsigned short int __port) {
	unsigned short int __value;
	__asm__ __volatile__ ("inw %w1,%0\noutb %%al,$0x80":"=a" (__value):"Nd" (__port));
	return __value;
}

__MLIBC_INLINE_DEFINITION unsigned int inl(unsigned short int __port) {
	unsigned int __value;
	__asm__ __volatile__ ("inl %w1,%0":"=a" (__value):"Nd" (__port));
	return __value;
}

__MLIBC_INLINE_DEFINITION unsigned int inl_p(unsigned short int __port) {
	unsigned int __value;
	__asm__ __volatile__ ("inl %w1,%0\noutb %%al,$0x80":"=a" (__value):"Nd" (__port));
	return __value;
}

__MLIBC_INLINE_DEFINITION void outb(unsigned char value, unsigned short int __port) {
	__asm__ __volatile__ ("outb %b0,%w1": :"a" (value), "Nd" (__port));
}

__MLIBC_INLINE_DEFINITION void outb_p(unsigned char __value, unsigned short int __port) {
	__asm__ __volatile__ ("outb %b0,%w1\noutb %%al,$0x80": :"a" (__value), "Nd" (__port));
}

__MLIBC_INLINE_DEFINITION void outw(unsigned short int __value, unsigned short int __port) {
	__asm__ __volatile__ ("outw %w0,%w1": :"a" (__value), "Nd" (__port));
}

__MLIBC_INLINE_DEFINITION void outw_p(unsigned short int __value, unsigned short int __port) {
	__asm__ __volatile__ ("outw %w0,%w1\noutb %%al,$0x80": :"a" (__value), "Nd" (__port));
}

__MLIBC_INLINE_DEFINITION void outl(unsigned int __value, unsigned short int __port) {
	__asm__ __volatile__ ("outl %0,%w1": :"a" (__value), "Nd" (__port));
}

__MLIBC_INLINE_DEFINITION void outl_p(unsigned int __value, unsigned short int __port) {
	__asm__ __volatile__ ("outl %0,%w1\noutb %%al,$0x80": :"a" (__value), "Nd" (__port));
}

__MLIBC_INLINE_DEFINITION void insb(unsigned short int __port, void *__addr, unsigned long int __count) {
	__asm__ __volatile__ ("cld ; rep ; insb":"=D" (__addr), "=c" (__count) :"d" (__port), "0" (__addr), "1" (__count));
}

__MLIBC_INLINE_DEFINITION void insw(unsigned short int __port, void *__addr, unsigned long int __count) {
	__asm__ __volatile__ ("cld ; rep ; insw":"=D" (__addr), "=c" (__count) :"d" (__port), "0" (__addr), "1" (__count));
}

__MLIBC_INLINE_DEFINITION void insl(unsigned short int __port, void *__addr, unsigned long int __count) {
	__asm__ __volatile__ ("cld ; rep ; insl":"=D" (__addr), "=c" (__count) :"d" (__port), "0" (__addr), "1" (__count));
}

__MLIBC_INLINE_DEFINITION void outsb(unsigned short int __port, const void *__addr, unsigned long int __count) {
	__asm__ __volatile__ ("cld ; rep ; outsb":"=S" (__addr), "=c" (__count) :"d" (__port), "0" (__addr), "1" (__count));
}

__MLIBC_INLINE_DEFINITION void outsw(unsigned short int __port, const void *__addr, unsigned long int __count) {
	__asm__ __volatile__ ("cld ; rep ; outsw":"=S" (__addr), "=c" (__count) :"d" (__port), "0" (__addr), "1" (__count));
}

__MLIBC_INLINE_DEFINITION void outsl(unsigned short int __port, const void *__addr, unsigned long int __count) {
	__asm__ __volatile__ ("cld ; rep ; outsl":"=S" (__addr), "=c" (__count) :"d" (__port), "0" (__addr), "1" (__count));
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IO_H */
