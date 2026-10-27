static char order[8];
// Keep constructor writes at runtime to avoid ordering issues.
static volatile int count;

void record(char c) {
	order[count++] = c;
}

const char *get_order(void) {
	return order;
}

[[gnu::constructor]] static void init_bar(void) {
	record('b');
}
