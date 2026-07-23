#include <assert.h>
#include <string.h>

// strtok() is specified by ISO C; strtok_r() is the POSIX reentrant variant.
// Reference: https://pubs.opengroup.org/onlinepubs/007904975/functions/strtok.html

static void test_strtok(void) {
	// POSIX defines tokens as nonempty spans separated by one or more delimiter
	// characters. Leading, trailing, and consecutive delimiters must be skipped.
	char input[] = ",;alpha:: beta;gamma;;";
	char *token = strtok(input, ",; :");
	assert(token == input + 2);
	assert(!strcmp(token, "alpha"));

	token = strtok(NULL, ",; :");
	assert(!strcmp(token, "beta"));
	token = strtok(NULL, ",; :");
	assert(!strcmp(token, "gamma"));
	token = strtok(NULL, ",; :");
	assert(token == NULL);

	// strtok() replaces the delimiter ending each returned token with '\0', and
	// returns pointers into the caller-provided mutable string.
	assert(input[7] == '\0');
	assert(input[14] == '\0');

	// POSIX explicitly permits an empty delimiter string: the remainder of the
	// input is one token, followed by end-of-tokenization.
	char no_delimiters[] = "one two";
	token = strtok(no_delimiters, "");
	assert(token == no_delimiters);
	assert(!strcmp(token, "one two"));
	assert(strtok(NULL, "") == NULL);

	// No non-delimiter span means no token, both for an empty string and for a
	// string made entirely of delimiters.
	char empty[] = "";
	assert(strtok(empty, ",") == NULL);
	char only_delimiters[] = ",;,:";
	assert(strtok(only_delimiters, ",;:") == NULL);

	// Passing a new non-null input resets strtok()'s internal sequence state;
	// the following NULL-input call continues with the new string.
	char first[] = "first second";
	char second[] = "third fourth";
	assert(!strcmp(strtok(first, " "), "first"));
	assert(!strcmp(strtok(second, " "), "third"));
	assert(!strcmp(strtok(NULL, " "), "fourth"));
}

static void test_strtok_r(void) {
	// POSIX strtok_r() stores continuation state in the caller's save pointer,
	// allowing two tokenization sequences to proceed independently.
	char first[] = "one,:two";
	char second[] = "red;:blue";
	char ignored_first_state[] = "ignored";
	char ignored_second_state[] = "ignored";
	char *first_save = ignored_first_state;
	char *second_save = ignored_second_state;

	// POSIX does not require the caller to initialize *state before the first
	// call; the supplied string starts a new tokenization sequence.
	char *token = strtok_r(first, ",", &first_save);
	assert(token == first);
	assert(!strcmp(token, "one"));

	// The first call supplies a string; subsequent calls supply NULL and reuse
	// the saved continuation pointer. The separator string may change on each
	// call, so the next calls skip the colon after the first delimiter.
	token = strtok_r(second, ";", &second_save);
	assert(token == second);
	assert(!strcmp(token, "red"));

	token = strtok_r(NULL, ":", &first_save);
	assert(!strcmp(token, "two"));
	token = strtok_r(NULL, ":", &second_save);
	assert(!strcmp(token, "blue"));
	assert(strtok_r(NULL, ":", &first_save) == NULL);
	assert(strtok_r(NULL, ":", &second_save) == NULL);

	// Delimiters are characters from a null-terminated byte string. This also
	// exercises values above 0x7f, which must not be confused with EOF-like or
	// sign-extended values by the implementation.
	char high_byte[] = { 'a', (char) 0xff, 'b', '\0' };
	char high_delimiter[] = { (char) 0xff, '\0' };
	char *save = NULL;
	assert(!strcmp(strtok_r(high_byte, high_delimiter, &save), "a"));
	assert(!strcmp(strtok_r(NULL, high_delimiter, &save), "b"));
	assert(strtok_r(NULL, high_delimiter, &save) == NULL);
}

int main(void) {
	test_strtok();
	test_strtok_r();
	return 0;
}
