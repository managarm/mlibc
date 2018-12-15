#ifndef MLIBC_ELF_STARTUP
#define MLIBC_ELF_STARTUP

namespace mlibc {

struct exec_stack_data {
	int argc;
	char **argv;
	char **envp;
};

void parse_exec_stack(void *sp, exec_stack_data *data);

} // namespace mlibc

#endif // MLIBC_ELF_STARTUP
