// In a static build mlibc's init/fini array entries used to be ordered after
// the program's, leaving stdio and environ unusable in constructors and already
// torn down in destructors.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static int ctor_prio_ran;
static int ctor_default_ran;
static int path_seen_in_ctor;

static void check_stdio(const char *phase) {
	assert(stdout != NULL);
	assert(fprintf(stdout, "[%s]\n", phase) > 0);
	assert(fflush(stdout) == 0);
}

static void quick_exit_handler(void) { }

// 101 is the lowest priority a program may use, so this runs before any other
// user code.
__attribute__((constructor(101))) static void ctor_prio(void) {
	check_stdio("ctor(101)");
	path_seen_in_ctor = (getenv("PATH") != NULL);
	assert(at_quick_exit(quick_exit_handler) == 0);
	ctor_prio_ran = 1;
}

__attribute__((constructor)) static void ctor_default(void) {
	assert(ctor_prio_ran);
	check_stdio("ctor(default)");
	ctor_default_ran = 1;
}

__attribute__((destructor)) static void dtor_default(void) {
	check_stdio("dtor(default)");
}

// Runs last: prioritised fini entries come first in the array, which is walked
// in reverse.
__attribute__((destructor(101))) static void dtor_prio(void) {
	check_stdio("dtor(101)");
}

int main(void) {
	assert(ctor_prio_ran);
	assert(ctor_default_ran);
	check_stdio("main");

	// getenv() must work in a constructor too, not just here.
	if (getenv("PATH") != NULL)
		assert(path_seen_in_ctor);

	return 0;
}
