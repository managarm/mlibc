void record(char c);

[[gnu::constructor]] static void init_foo(void) {
	record('f');
}

// Referenced by the executable so that libfoo is not dropped by --as-needed.
void foo(void) {}
