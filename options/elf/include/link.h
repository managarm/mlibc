#ifndef _LINK_H
#define _LINK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <elf.h>
#include <stddef.h>
    
#if defined(__x86_64__) || defined(__aarch64__)
#	define ElfW(type) Elf64_ ## type
#elif defined(__i386__)
#	define ElfW(type) Elf32_ ## type
#else
# 	error Unknown architecture
#endif
    
struct dl_phdr_info {
	ElfW(Addr) dlpi_addr;
	const char *dlpi_name;
	const ElfW(Phdr) *dlpi_phdr;
	ElfW(Half) dlpi_phnum;
	unsigned long long int dlpi_adds;
	unsigned long long int dlpi_subs;
	size_t dlpi_tls_modid;
	void *dlpi_tls_data;
};

struct link_map {
	Elf64_Addr l_addr;
	char *l_name;
	ElfW(Dyn) *l_ld;
	struct link_map *l_next, *l_prev;
};

struct r_debug {
	int r_version;
	struct link_map *r_map;
	Elf64_Addr r_brk;
	enum { RT_CONSISTENT, RT_ADD, RT_DELETE } r_state;
	Elf64_Addr r_ldbase;
};

int dl_iterate_phdr(int (*callback)(struct dl_phdr_info*, size_t, void*), void* data);

#ifdef __cplusplus
}
#endif

#endif // _LINK_H
