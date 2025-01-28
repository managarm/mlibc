#include <stddef.h>
#include <dlfcn.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#define LIBBAR "libnative-bar.so"
#else
#define LIBFOO "libfoo.so"
#define LIBBAR "libbar.so"
#endif

typedef char *strfn(void);

int main() {
	// We haven't dlopen'd these libs yet, so symbol resolution should fail.
	assert(dlsym(RTLD_DEFAULT, "foo") == NULL);
	assert(dlsym(RTLD_DEFAULT, "bar") == NULL);

	assert(!dlopen(LIBFOO, RTLD_NOLOAD));
	assert(!dlopen(LIBBAR, RTLD_NOLOAD));

	void *foo_handle = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(foo_handle);
	assert(dlopen(LIBFOO, RTLD_NOLOAD | RTLD_NOW));

	strfn *foo_sym = dlsym(foo_handle, "foo");
	assert(foo_sym);
	assert(foo_sym());
	assert(!strcmp(foo_sym(), "foo"));

	strfn *foo_global_sym = dlsym(foo_handle, "foo_global");
	assert(foo_global_sym);
	assert(foo_global_sym());
	assert(!strcmp(foo_global_sym(), "foo global"));

	assert(dlsym(foo_handle, "doesnotexist") == NULL);

	// Nested opening should work
	assert(dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW) == foo_handle);
	assert(dlopen(LIBFOO, RTLD_NOLOAD | RTLD_NOW));

	// Since we've loaded the same library twice, the addresses should be the same
	assert(dlsym(foo_handle, "foo") == foo_sym);
	assert(dlsym(foo_handle, "foo_global") == foo_global_sym);

	// libfoo was opened with RTLD_LOCAL, so we shouldn't be able to lookup
	// its symbols in the global namespace.
	assert(dlsym(RTLD_DEFAULT, "foo") == NULL);

	{
		void *bar_handle = dlopen(LIBBAR, RTLD_GLOBAL | RTLD_NOW);
		assert(bar_handle);
		assert(dlopen(LIBBAR, RTLD_NOLOAD | RTLD_NOW));
		
		strfn *bar_sym = dlsym(bar_handle, "bar");
		assert(bar_sym);
		assert(bar_sym());
		assert(!strcmp(bar_sym(), "bar"));
		
		strfn *bar_calls_foo_sym = dlsym(bar_handle, "bar_calls_foo");
		assert(bar_calls_foo_sym);
		assert(bar_calls_foo_sym());
		assert(!strcmp(bar_calls_foo_sym(), "foo"));
		
		strfn *bar_calls_foo_global_sym = dlsym(bar_handle, "bar_calls_foo_global");
		assert(bar_calls_foo_global_sym);
		assert(bar_calls_foo_global_sym());
		assert(!strcmp(bar_calls_foo_global_sym(), "foo global"));

		// libbar was opened with RTLD_GLOBAL, so we can find symbols by
		// searching in the global scope.
		strfn *new_bar_sym = dlsym(RTLD_DEFAULT, "bar");
		assert(new_bar_sym);
		assert(new_bar_sym == bar_sym);

		// Note that we loaded libbar with RTLD_GLOBAL, which should pull
		// in libfoo's symbols globally too.
		strfn *new_foo_sym = dlsym(RTLD_DEFAULT, "foo");
		assert(new_foo_sym);
		assert(new_foo_sym == foo_sym);

		assert(dlclose(bar_handle) == 0);
	}

	assert(dlclose(foo_handle) == 0);
	assert(dlclose(foo_handle) == 0);

	return 0;
}
