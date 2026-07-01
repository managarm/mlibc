#include <assert.h>
#include <locale.h>
#include <regex.h>

static int matches(const regex_t *rx, const char *subject) {
	int rc = regexec(rx, subject, 0, NULL, 0);
	assert(rc == 0 || rc == REG_NOMATCH);
	return rc == 0;
}

int main(void) {
	setlocale(LC_ALL, "C");

	regex_t rx;
	int rc = regcomp(&rx, "_rels/\\.rels", REG_EXTENDED | REG_NEWLINE);
	assert(!rc);

	assert(matches(&rx, "_rels/.rels"));     /* exact */
	assert(matches(&rx, "_rels/.relsXYZ"));  /* trailing ASCII */
	assert(matches(&rx, "_rels/.rels\x7f")); /* 0x7f is still ASCII */

	assert(matches(&rx, "_rels/.rels\x80"));
	assert(matches(&rx, "_rels/.rels\xff"));

	assert(matches(&rx, "pre\xad_rels/.rels"));

	assert(!matches(&rx, "\x80\xff nothing here"));

	regfree(&rx);
	return 0;
}
