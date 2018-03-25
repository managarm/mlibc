
#include <bits/ensure.h>
#include <fenv.h>

int feclearexcept(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fegetenv(fenv_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fegetexceptflag(fexcept_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fegetround(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int feholdexcept(fenv_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int feraiseexcept(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fesetenv(const fenv_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fesetexceptflag(const fexcept_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fesetround(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fetestexcept(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int feupdateenv(const fenv_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
