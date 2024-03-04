char *foo_baz_conflict(void);

char *bar_calls_foo_baz_conflict() {
	return foo_baz_conflict();
}
