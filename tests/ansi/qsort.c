#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct record {
	int key;
	char value[12];
};

struct sort_context {
	int descending;
	unsigned int calls;
	unsigned int magic;
};

static int compare_ints(const void *p1, const void *p2) {
	int a = *(const int *) p1;
	int b = *(const int *) p2;
	if (a < b)
		return -100;
	if (a > b)
		return 100;
	return 0;
}

static int compare_bytes(const void *p1, const void *p2) {
	unsigned char a = *(const unsigned char *) p1;
	unsigned char b = *(const unsigned char *) p2;
	if (a < b)
		return -100;
	if (a > b)
		return 100;
	return 0;
}

static int compare_records(const void *p1, const void *p2, void *ctx) {
	const struct record *a = p1;
	const struct record *b = p2;
	struct sort_context *context = ctx;
	assert(context->magic == 0xDEADBEEF);
	context->calls++;

	if (a->key == b->key)
		return 0;
	if (context->descending)
		return a->key < b->key ? 100 : -100;
	return a->key < b->key ? -100 : 100;
}

static int compare_strings(const void *p1, const void *p2, void *ctx) {
	struct sort_context *context = ctx;
	assert(context->magic == 0xDEADBEEF);
	context->calls++;
	return strcmp(*(const char *const *) p1, *(const char *const *) p2);
}

static void assert_record_values(const struct record *records, size_t count) {
	static const char *expected[] = { "seven", "minus3", "four", "four-b", "zero", "nineteen" };
	for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
		int found = 0;
		for (size_t j = 0; j < count; j++) {
			if (!strcmp(records[j].value, expected[i])) {
				found = 1;
				break;
			}
		}
		assert(found);
	}
}

static void test_qsort(void) {
	// ISO C qsort(): ordinary integer ordering, duplicates, and non-trivial
	// comparator return values (the comparator need not return +/-1).
	int values[] = { 7, -3, 4, 4, 0, 19, -10, 2 };
	const int expected[] = { -10, -3, 0, 2, 4, 4, 7, 19 };
	qsort(values, sizeof(values) / sizeof(values[0]), sizeof(values[0]), compare_ints);
	assert(!memcmp(values, expected, sizeof(values)));

	// Sorting an already reverse-ordered input exercises repeated partitioning.
	int reverse[] = { 5, 4, 3, 2, 1, 0 };
	qsort(reverse, sizeof(reverse) / sizeof(reverse[0]), sizeof(reverse[0]), compare_ints);
	for (size_t i = 0; i < sizeof(reverse) / sizeof(reverse[0]); i++)
		assert(reverse[i] == (int) i);

	// qsort() must honor arbitrary element sizes, including one-byte objects.
	unsigned char bytes[] = { 3, 1, 2, 0, 255 };
	qsort(bytes, sizeof(bytes), sizeof(bytes[0]), compare_bytes);
	assert(bytes[0] == 0 && bytes[1] == 1 && bytes[2] == 2 && bytes[3] == 3 && bytes[4] == 255);

	// A one-element range requires no comparisons or swaps.
	int one[] = { 42 };
	qsort(one, 1, sizeof(one[0]), compare_ints);
	assert(one[0] == 42);

	// A zero-count call must not access or modify the base object.
	int empty[1] = { 42 };
	qsort(empty, 0, sizeof(empty[0]), compare_ints);
	assert(empty[0] == 42);
}

static void test_qsort_r(void) {
	// POSIX qsort_r(): the callback receives a caller-provided
	// context, and sorting must move complete multi-field records.
	struct {
		unsigned char before[16];
		struct record records[6];
		unsigned char after[16];
	} guarded = {
		{ 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
		  0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5 },
		{
			{ 7, "seven" }, { -3, "minus3" }, { 4, "four" },
			{ 4, "four-b" }, { 0, "zero" }, { 19, "nineteen" }
		},
		{ 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
		  0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A }
	};
	struct sort_context context = { 0, 0, 0xDEADBEEF };
	qsort_r(guarded.records, sizeof(guarded.records) / sizeof(guarded.records[0]),
			sizeof(guarded.records[0]),
			compare_records, &context);
	assert(context.calls > 0);
	for (size_t i = 1; i < sizeof(guarded.records) / sizeof(guarded.records[0]); i++)
		assert(guarded.records[i - 1].key <= guarded.records[i].key);
	assert_record_values(guarded.records, sizeof(guarded.records) / sizeof(guarded.records[0]));

	// Reuse the same context to verify that callback state can select a
	// different ordering direction.
	context.descending = 1;
	context.calls = 0;
	qsort_r(guarded.records, sizeof(guarded.records) / sizeof(guarded.records[0]),
			sizeof(guarded.records[0]),
			compare_records, &context);
	assert(context.calls > 0);
	for (size_t i = 1; i < sizeof(guarded.records) / sizeof(guarded.records[0]); i++)
		assert(guarded.records[i - 1].key >= guarded.records[i].key);
	assert_record_values(guarded.records, sizeof(guarded.records) / sizeof(guarded.records[0]));
	for (size_t i = 0; i < sizeof(guarded.before); i++) {
		assert(guarded.before[i] == 0xA5);
		assert(guarded.after[i] == 0x5A);
	}

	// qsort_r() also handles pointer-sized elements and string comparators.
	char *strings[] = { "xyz", "abc", "ghi", "def" };
	context.descending = 0;
	context.calls = 0;
	qsort_r(strings, sizeof(strings) / sizeof(strings[0]), sizeof(strings[0]),
			compare_strings, &context);
	assert(context.calls > 0);
	assert(!strcmp(strings[0], "abc"));
	assert(!strcmp(strings[1], "def"));
	assert(!strcmp(strings[2], "ghi"));
	assert(!strcmp(strings[3], "xyz"));

	// As with qsort(), a zero-count range must not invoke the comparator or
	// modify the element passed as the base address.
	struct record empty[] = { { 42, "unchanged" } };
	context.calls = 0;
	qsort_r(empty, 0, sizeof(empty[0]), compare_records, &context);
	assert(context.calls == 0);
	assert(empty[0].key == 42);
	assert(!strcmp(empty[0].value, "unchanged"));
}

int main() {
	test_qsort();
	test_qsort_r();
	return 0;
}
