#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
int main(void) {
	char *testString = "mlibc is the best best best libc";
	char *pattern = "\\(be[a-z]t\\) \\1";

	regex_t reg;
	int rc = regcomp(&reg, pattern, 0);
	assert(!rc);

	regmatch_t matches[2];
	rc = regexec(&reg, testString, 2, matches, 0);
	assert(!rc);

	printf("Whole pattern: \"%.*s\" at %zd-%zd.\n",
		(int)(matches[0].rm_eo - matches[0].rm_so), &testString[matches[0].rm_so],
		(ssize_t)matches[0].rm_so, (ssize_t)(matches[0].rm_eo - 1));
	assert(matches[0].rm_so == 13 && matches[0].rm_eo == 22);

	printf("Substring: \"%.*s\" at %zd-%zd.\n",
		(int)(matches[1].rm_eo - matches[1].rm_so), &testString[matches[1].rm_so],
		(ssize_t)matches[1].rm_so, (ssize_t)matches[1].rm_eo - 1);
	assert(matches[1].rm_so == 13 && matches[1].rm_eo == 17);

	regfree(&reg);
	return 0;
}
