#include <dlfcn.h>

typedef char *charFn(void);

__attribute__((weak))
char *definedInBoth() {
	return "bar";
}

charFn *barGetDefault() {
	return (charFn *)dlsym(RTLD_DEFAULT, "definedInBoth");
}

charFn *barGetNext() {
	return (charFn *)dlsym(RTLD_NEXT, "definedInBoth");
}
