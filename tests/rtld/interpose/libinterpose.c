int foo(void) {
	return 2;
}

// Referenced by the executable so that libinterpose is not dropped by --as-needed.
void interpose(void) {}
