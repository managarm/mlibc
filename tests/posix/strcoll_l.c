#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#define TEST(loc, s1, s2, cmp) do { \
	int res = strcoll_l(s1, s2, loc); \
	fprintf(stderr, "Comparing '%s' vs '%s': result = %d\n", s1, s2, res); \
	assert(res cmp); \
} while(0);

int main() {
	setlocale(LC_ALL, "C.utf8");

	locale_t german = newlocale(LC_COLLATE_MASK, "de_DE.utf8", (locale_t)0);
	assert(german);
	locale_t french = newlocale(LC_COLLATE_MASK, "fr_FR.utf8", (locale_t)0);
	assert(french);

	TEST(german, "Aal", "Ära", == -115);
	TEST(german, "Ära", "Bär", == -13);
	TEST(german, "Müller", "Müller", == 0);
	TEST(german, "abc", "ABC", == -5);
	TEST(german, "essen", "Essen", == -5);
	TEST(german, "co-op", "coop", == -27);
	TEST(german, "co\u200Bop", "co\u200B\u200B\u200Bop", == 157);
	TEST(german, "\u212B", "\u00c5", == 1);
	TEST(german, "😁", "🙂", == -1);
	TEST(german, "😁", "🤕", == -9);
	TEST(german, "😁", "ß", == -1);

	TEST(french, "cote", "côte", == -7);
	TEST(french, "ete", "été", == -4);

	freelocale(german);
	freelocale(french);

	return 0;
}
