#ifndef _MLIBC_ELF_STARTUP_H
#define _MLIBC_ELF_STARTUP_H

namespace mlibc {

struct exec_stack_data {
	int argc;
	char **argv;
	char **envp;
};

extern exec_stack_data entry_stack;

#ifndef __MLIBC_ABI_ONLY

void parse_exec_stack(void *sp, exec_stack_data *data);

void set_startup_data(int argc, char **argv, char **envp);

#endif /* !__MLIBC_ABI_ONLY */

} /* namespace mlibc */

#endif /* _MLIBC_ELF_STARTUP_H */
