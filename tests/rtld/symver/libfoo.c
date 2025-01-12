int foo_v1(void) {
	return 1;
}

int foo_v2(void) {
	return 2;
}

int foo_v3(void) {
	return 3;
}

asm(".symver foo_v1, foo@FOO_1");
asm(".symver foo_v2, foo@FOO_2");
asm(".symver foo_v3, foo@@FOO_3"); // default version (due to @@ vs @)
