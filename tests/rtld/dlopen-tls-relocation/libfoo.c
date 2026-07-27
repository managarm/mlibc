static int object;

_Thread_local void *pointer = &object;

void *get_tls_pointer(void) {
	return pointer;
}

void *get_object_pointer(void) {
	return &object;
}
