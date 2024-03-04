#include <dlfcn.h>

typedef char *charFn(void);

__attribute__((weak))
char *definedInBoth() {
	return "foo";
}

charFn *fooGetDefault() {
	return (charFn *)dlsym(RTLD_DEFAULT, "definedInBoth");
}

charFn *fooGetNext() {
	return (charFn *)dlsym(RTLD_NEXT, "definedInBoth");
}

