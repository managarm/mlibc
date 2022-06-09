char *foo() {
	return "foo";
}

char global[] = "foo global";

char *foo_global() {
	return global;
}
