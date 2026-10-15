static char order[8];
static int count;

void record(char c) {
	order[count++] = c;
}

const char *get_order(void) {
	return order;
}

[[gnu::constructor]] static void init_bar(void) {
	record('b');
}
