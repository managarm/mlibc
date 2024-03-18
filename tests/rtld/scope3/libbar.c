int g = 1;

int call_foo(void);

int call_bar(void) {
	return call_foo();
}

