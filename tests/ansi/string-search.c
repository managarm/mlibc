#include <assert.h>
#include <stddef.h>
#include <string.h>

// ISO C string-search functions:
// https://en.cppreference.com/w/c/string/byte
// GNU strchrnul() extension:
// https://man7.org/linux/man-pages/man3/strchrnul.3.html

static void test_strstr(void) {
	// A nonempty needle matches at the first occurrence and returns a pointer
	// into the original haystack.
	char haystack[] = "abcabc";
	assert(strstr(haystack, "abc") == haystack);
	assert(strstr(haystack + 1, "abc") == haystack + 3);

	// A missing needle and a needle longer than the remaining suffix return NULL.
	assert(strstr(haystack, "abd") == NULL);
	assert(strstr(haystack + 4, "abc") == NULL);

	// The empty needle matches at the beginning, including for an empty string.
	char empty[] = "";
	assert(strstr(haystack, "") == haystack);
	assert(strstr(empty, "") == empty);

	// String functions stop at the first embedded null byte.
	char embedded[] = { 'a', 'b', '\0', 'c', 'd', '\0' };
	assert(strstr(embedded, "ab") == embedded);
	assert(strstr(embedded, "cd") == NULL);
}

static void test_strpbrk(void) {
	// strpbrk() returns the first character from the accept set.
	char text[] = "hello world";
	assert(strpbrk(text, "ow") == text + 4);
	assert(strpbrk(text, "d") == text + 10);

	// No matching character and an empty accept set both produce NULL.
	assert(strpbrk(text, "xyz") == NULL);
	assert(strpbrk(text, "") == NULL);

	// Search sets are byte-oriented, including bytes with the high bit set.
	char high_byte[] = { 'a', (char) 0xff, 'b', '\0' };
	char high_set[] = { (char) 0xff, '\0' };
	assert(strpbrk(high_byte, high_set) == high_byte + 1);
}

static void test_strcspn(void) {
	// strcspn() counts the prefix containing no character from the reject set.
	assert(strcspn("abc:def", ":") == 3);
	assert(strcspn("abc", "a") == 0);
	assert(strcspn("abc", "xyz") == 3);

	// An empty reject set accepts the whole string, and embedded nulls end it.
	assert(strcspn("abc", "") == 3);
	char embedded[] = { 'a', 'b', '\0', ':', '\0' };
	assert(strcspn(embedded, ":") == 2);

	char high_byte[] = { 'a', (char) 0xff, 'b', '\0' };
	char high_set[] = { (char) 0xff, '\0' };
	assert(strcspn(high_byte, high_set) == 1);
}

static void test_strspn(void) {
	// strspn() counts the initial prefix containing only accepted characters.
	assert(strspn("12345abc", "0123456789") == 5);
	assert(strspn("abc123", "abc") == 3);
	assert(strspn("x123", "0123456789") == 0);

	// An empty accept set accepts nothing, while an embedded null ends the scan.
	assert(strspn("abc", "") == 0);
	char embedded[] = { '1', '2', '\0', '3', '\0' };
	assert(strspn(embedded, "0123456789") == 2);

	char high_prefix[] = { (char) 0xff, (char) 0xff, 'a', '\0' };
	char high_set[] = { (char) 0xff, '\0' };
	assert(strspn(high_prefix, high_set) == 2);
}

static void test_strchrnul(void) {
	// GNU strchrnul() returns the matching character, or the terminating null
	// byte when no match exists.
	char text[] = "abc";
	assert(strchrnul(text, 'b') == text + 1);
	assert(strchrnul(text, 'x') == text + 3);
	assert(strchrnul(text, '\0') == text + 3);

	// The search value is converted to unsigned char, so high-bit bytes and
	// values outside the unsigned-char range are handled consistently.
	char high_byte[] = { 'a', (char) 0xff, 'b', '\0' };
	assert(strchrnul(high_byte, 0xff) == high_byte + 1);
	assert(strchrnul(high_byte, 0x1ff) == high_byte + 1);
}

int main(void) {
	test_strstr();
	test_strpbrk();
	test_strcspn();
	test_strspn();
	test_strchrnul();
	return 0;
}
