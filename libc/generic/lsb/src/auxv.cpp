
#include <errno.h>
#include <sys/auxv.h>

#include <mlibc/ensure.h>

struct Auxiliary {
	// the ABI supplement specifies type to be an 'int' but
	// Linux uses unsigned long. It does not really matter though
	// because 'int' would be padded anyways.
	unsigned long type;
	union {
		long longValue;
		void *pointerValue;
	};
};

extern "C" Auxiliary *__rtdl_auxvector();

int peekauxval(unsigned long type, unsigned long *value) {
	// parse the auxiliary vector.
	auto element = __rtdl_auxvector();
	while(true) {
		if(element->type == AT_NULL) {
			errno = ENOENT;
			return -1;
		}else if(element->type == type) {
			*value = element->longValue;
			return 0;
		}
		element++;
	}
}

unsigned long getauxval(unsigned long type) {
	unsigned long value = 0;
	if(peekauxval(type, &value))
		return 0;
	return value;
}

