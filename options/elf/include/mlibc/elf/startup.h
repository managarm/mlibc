#ifndef MLIBC_ELF_STARTUP
#define MLIBC_ELF_STARTUP

void __mlibc_run_constructors();

namespace mlibc {

struct exec_stack_data {
	int argc;
	char **argv;
	char **envp;
};

void parse_exec_stack(void *sp, exec_stack_data *data);

void set_startup_data(int argc, char **argv, char **envp);

} // namespace mlibc

#endif // MLIBC_ELF_STARTUP
