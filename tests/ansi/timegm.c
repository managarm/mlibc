#include <time.h>
#include <stdio.h>
#include <assert.h>

int main() {
	struct tm soon = {};
	soon.tm_sec = 0;
	soon.tm_min = 0;
	soon.tm_hour = 0;
	soon.tm_mday = 1;
	soon.tm_mon = 0;
	soon.tm_year = 70;
	time_t result;
	time_t expected_result = 0;
	// This should be epoch.
	result = timegm(&soon);
	printf("epoch: %ld\n", result);
	assert(result == expected_result);

	soon.tm_sec = 12;
	soon.tm_min = 8;
	soon.tm_hour = 16;
	soon.tm_mday = 17;
	soon.tm_mon = 4;
	soon.tm_year = 122;
	expected_result = 1652803692;
	result = timegm(&soon);
	// On my host, this returned 1652803692, verify this.
	printf("epoch: %ld\n", result);
	assert(result == expected_result);

	soon.tm_sec = 45;
	soon.tm_min = 42;
	soon.tm_hour = 17;
	soon.tm_mday = 16;
	soon.tm_mon = 8;
	soon.tm_year = 69;
	expected_result = -9181035;
	result = timegm(&soon);
	// On my host, this returned -9181035, verify this.
	printf("epoch: %ld\n", result);
	assert(result == expected_result);

	return 0;
}
