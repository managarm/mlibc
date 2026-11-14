#include <mlibc/init-priority.hpp>

[[gnu::constructor(MLIBC_INIT_PRIORITY_ENTRY)]]
static void run_before_main() {
	// In case this is needed, a libc initialization code placed here will
	// be run by mlibc while initializing the program, before running other
	// constructors and entering the main() function.
}
