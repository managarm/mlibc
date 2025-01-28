char *foo(void) {
	return "foo";
}

char global[] = "foo global";

char *foo_global(void) {
	return global;
}
