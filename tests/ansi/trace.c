#include <stdio.h>

int main() {
	__MLIBC_TRACE();
	__MLIBC_TRACE("foo");
	__MLIBC_TRACE("bar %d", 1);
	__MLIBC_TRACE("baz %d %s", 1, "quux");
}
