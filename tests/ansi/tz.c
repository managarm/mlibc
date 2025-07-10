#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int daylight;
extern long timezone;
extern char *tzname[2];

int main() {
	tzset();
	printf("%s %s %ld %d\n", tzname[0], tzname[1], timezone, daylight);

	time_t now = time(NULL);
	struct tm *local = localtime(&now);

	// FIXME: printf formatting is broken so we can't do this in a test

	// Print the current timezone offset from UTC in seconds, considering DST
	// if (local) {
	// 	long offset = local->tm_gmtoff;
	// 	int hours = offset / 3600;
	// 	int minutes = (offset % 3600) / 60;
	// 	printf("UTC offset: %+03d:%02d\n", hours, abs(minutes));
	// }

	return 0;
}
