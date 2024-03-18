#include <string.h>
#include <assert.h>

typedef char *charFn(void);
charFn *fooGetDefault(void);
charFn *fooGetNext(void);
charFn *barGetDefault(void);
charFn *barGetNext(void);

int main() {
	charFn *ret;

	ret = fooGetDefault();
	assert(ret != NULL);
	assert(!strcmp(ret(), "foo"));

	ret = fooGetNext();
	assert(ret != NULL);
	assert(!strcmp(ret(), "bar"));

	ret = barGetDefault();
	assert(ret != NULL);
	assert(!strcmp(ret(), "foo"));

	assert(barGetNext() == NULL);

	return 0;
}
