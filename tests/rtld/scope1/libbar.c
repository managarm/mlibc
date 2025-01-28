char *foo(void);
char *foo_global(void);

char *bar(void) {
	return "bar";
}

char *bar_calls_foo(void) {
	return foo();
}

char *bar_calls_foo_global(void) {
	return foo_global();
}
