int g = 2;

int call_foo(void);

int call_baz(void) {
	return call_foo();
}
