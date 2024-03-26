#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>

#ifdef USE_HOST_LIBC
#define LIBBAR "libnative-bar.so"
#define LIBBAZ "libnative-baz.so"
#else
#define LIBBAR "libbar.so"
#define LIBBAZ "libbaz.so"
#endif

int main() {
	// In this test, we have bar -> foo and baz -> foo (where -> means 'depends on').
	// All three objects contain a definition of a symbol g. Bar calls into foo to retrieve
	// what foo thinks g is, but since bar appears earlier in the scope than foo, bar's copy
	// of g wins.
	//
	// Next, we load baz, which is identical to bar. When baz calls into foo to retrieve g,
	// foo still sees bar's definition of g, so bar's copy of g wins.
	//
	// Swapping the load order of bar and baz should therefore change the value of g which
	// foo sees. This behaviour is why dlmopen exists. If we ever implement that, we should
	// write a similar test and assert that the calls return different results.

	void *libbar = dlopen(LIBBAR, RTLD_LAZY | RTLD_LOCAL);
	int (*call_bar)(void) = dlsym(libbar, "call_bar");
	printf("call_bar: %d\n", call_bar());
	assert(call_bar() == 1);

	void *libbaz = dlopen(LIBBAZ, RTLD_LAZY | RTLD_LOCAL);
	int (*call_baz)(void) = dlsym(libbaz, "call_baz");
	printf("call_baz: %d\n", call_baz());
	assert(call_baz() == 1);


	dlclose(libbar);
	dlclose(libbaz);

	return 0;
}
