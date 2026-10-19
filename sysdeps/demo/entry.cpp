#include <mlibc/all-sysdeps.hpp>

// MLIBC_INIT_PRIORITY_ENTRY definition
#include <mlibc/init-priority.hpp>

namespace mlibc {

void Sysdeps<PreMainHook>::operator()() {
	// In case this is needed, a libc initialization code placed here will
	// be run by mlibc while initializing the program, after running all of the constructors, but before
	// entering the main() function.
}

} // namespace mlibc

[[gnu::constructor(MLIBC_INIT_PRIORITY_ENTRY)]]
static void run_before_constructors() {
	// This constructor runs after mlibc's environment and stdio initialization,
	// but before the previous hook.
	// The loader also runs DF_1_INITFIRST initializers and the executable's
	// preinit array before ordinary shared-library constructors.
	//
	// In static builds, this construct needs to be in some object file that has
	// a global variable or function that is referenced by the other object files;
	// for example, placing (even an empty) PreMainHook as above will ensure that
	// this constructor is pulled in by the linker.
}
