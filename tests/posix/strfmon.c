#include <assert.h>
#include <locale.h>
#include <monetary.h>
#include <stdio.h>
#include <string.h>

#define POSIX_TEST(fmt, a, b, c) do { \
	ret = strfmon(buf, sizeof(buf), fmt, 123.45); \
	if (strcmp(buf, a)) fprintf(stderr, "expected '%s' (%zu), got '%s' (%zd) for format '%s'\n", a, strlen(a), buf, ret, fmt); \
	assert(ret == (ssize_t) strlen(a)); \
	assert(!strcmp(buf, a)); \
	ret = strfmon(buf, sizeof(buf), fmt, -123.45); \
	if (strcmp(buf, b)) fprintf(stderr, "expected '%s' (%zu), got '%s' (%zd) for format '%s'\n", b, strlen(b), buf, ret, fmt); \
	assert(ret == (ssize_t) strlen(b)); \
	assert(!strcmp(buf, b)); \
	ret = strfmon(buf, sizeof(buf), fmt, 3456.781); \
	if (strcmp(buf, c)) fprintf(stderr, "expected '%s' (%zu), got '%s' (%zd) for format '%s'\n", c, strlen(c), buf, ret, fmt); \
	assert(ret == (ssize_t) strlen(c)); \
	assert(!strcmp(buf, c)); \
} while(0)

int main() {
	setlocale(LC_ALL, "C");

	char buf[128];
	ssize_t ret = strfmon(buf, sizeof(buf), "%%\n");
	assert(ret == 2);
	assert(!strcmp(buf, "%\n"));

	ret = strfmon(buf, sizeof(buf), "%i %n", 12.34, 42.0);
	fprintf(stderr, "'%s' (ret = %zd)\n", buf, ret);
	assert(ret == 11);
	assert(!strcmp(buf, "12.34 42.00"));

	setlocale(LC_ALL, "de_DE.utf8");

	ret = strfmon(buf, sizeof(buf), "%i %n", 12.34, 42.0);
	fprintf(stderr, "'%s' (ret = %zd)\n", buf, ret);
	assert(ret == 19);
	assert(!strcmp(buf, "12,34 EUR 42,00 €"));

	// test specifying a field width
	ret = strfmon(buf, sizeof(buf), "%12i", 12.34);
	fprintf(stderr, "'%s' (ret = %zd)\n", buf, ret);
	assert(ret == 12);
	assert(!strcmp(buf, "   12,34 EUR"));

	// test specifying a field width with left-justification
	ret = strfmon(buf, sizeof(buf), "%-12i", 12.34);
	fprintf(stderr, "'%s' (ret = %zd)\n", buf, ret);
	assert(ret == 12);
	assert(!strcmp(buf, "12,34 EUR   "));

	setlocale(LC_ALL, "en_US.utf8");

	POSIX_TEST("%n", "$123.45", "-$123.45", "$3,456.78");
	POSIX_TEST("%11n", "    $123.45", "   -$123.45", "  $3,456.78");
	POSIX_TEST("%#5n", " $   123.45", "-$   123.45", " $ 3,456.78");
	POSIX_TEST("%=*#5n", " $***123.45", "-$***123.45", " $*3,456.78");
	POSIX_TEST("%=0#5n", " $000123.45", "-$000123.45", " $03,456.78");
	POSIX_TEST("%^#5n", " $  123.45", "-$  123.45", " $ 3456.78");
	POSIX_TEST("%^#5.0n", " $  123", "-$  123", " $ 3457");
	POSIX_TEST("%^#5.4n", " $  123.4500", "-$  123.4500", " $ 3456.7810");
	POSIX_TEST("%(#5n", " $   123.45", "($   123.45)", " $ 3,456.78");
	POSIX_TEST("%!(#5n", "    123.45", "(   123.45)", "  3,456.78");
	POSIX_TEST("%-14#5.4n", " $   123.4500 ", "-$   123.4500 ", " $ 3,456.7810 ");
	POSIX_TEST("%14#5.4n", "  $   123.4500", " -$   123.4500", "  $ 3,456.7810");

	setlocale(LC_ALL, "de_DE");

	POSIX_TEST("%n", "123,45 EUR", "-123,45 EUR", "3.456,78 EUR");
	POSIX_TEST("%14n", "    123,45 EUR", "   -123,45 EUR", "  3.456,78 EUR");
	POSIX_TEST("%#5n", "    123,45 EUR", "-   123,45 EUR", "  3.456,78 EUR");
	POSIX_TEST("%=*#5n", " ***123,45 EUR", "-***123,45 EUR", " *3.456,78 EUR");
	POSIX_TEST("%=0#5n", " 000123,45 EUR", "-000123,45 EUR", " 03.456,78 EUR");
	POSIX_TEST("%^#5n", "   123,45 EUR", "-  123,45 EUR", "  3456,78 EUR");
	POSIX_TEST("%^#5.0n", "   123 EUR", "-  123 EUR", "  3457 EUR");
	POSIX_TEST("%^#5.4n", "   123,4500 EUR", "-  123,4500 EUR", "  3456,7810 EUR");
	POSIX_TEST("%(#5n", "    123,45 EUR", "(   123,45 EUR)", "  3.456,78 EUR");
	POSIX_TEST("%!(#5n", "    123,45", "(   123,45)", "  3.456,78");
	POSIX_TEST("%-14#5.4n", "    123,4500 EUR", "-   123,4500 EUR", "  3.456,7810 EUR");
	POSIX_TEST("%14#5.4n", "    123,4500 EUR", "-   123,4500 EUR", "  3.456,7810 EUR");

	setlocale(LC_ALL, "de_DE.utf8");

	POSIX_TEST("%n", "123,45 €", "-123,45 €", "3.456,78 €");
	POSIX_TEST("%14n", "    123,45 €", "   -123,45 €", "  3.456,78 €");
	POSIX_TEST("%#5n", "    123,45 €", "-   123,45 €", "  3.456,78 €");
	POSIX_TEST("%=*#5n", " ***123,45 €", "-***123,45 €", " *3.456,78 €");
	POSIX_TEST("%=0#5n", " 000123,45 €", "-000123,45 €", " 03.456,78 €");
	POSIX_TEST("%^#5n", "   123,45 €", "-  123,45 €", "  3456,78 €");
	POSIX_TEST("%^#5.0n", "   123 €", "-  123 €", "  3457 €");
	POSIX_TEST("%^#5.4n", "   123,4500 €", "-  123,4500 €", "  3456,7810 €");
	POSIX_TEST("%(#5n", "    123,45 €", "(   123,45 €)", "  3.456,78 €");
	POSIX_TEST("%!(#5n", "    123,45", "(   123,45)", "  3.456,78");
	POSIX_TEST("%-14#5.4n", "    123,4500 €", "-   123,4500 €", "  3.456,7810 €");
	POSIX_TEST("%14#5.4n", "    123,4500 €", "-   123,4500 €", "  3.456,7810 €");

	return 0;
}
