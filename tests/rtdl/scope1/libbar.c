char *foo(void);
char *foo_global(void);

char *bar() {
	return "bar";
}

char *bar_calls_foo() {
	return foo();
}

char *bar_calls_foo_global() {
	return foo_global();
}
