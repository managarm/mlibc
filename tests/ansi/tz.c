#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int daylight;
extern long timezone;
extern char *tzname[2];

int main() {
	tzset();
	printf("%s %s %ld %d\n", tzname[0], tzname[1], timezone, daylight);

	time_t sometime_in_summer_in_the_north = 1752019200;
	struct tm *local = localtime(&sometime_in_summer_in_the_north);

	// Print the current timezone offset from UTC in seconds, considering DST
	if (local) {
		long offset = local->tm_gmtoff;
		int hours = offset / 3600;
		int minutes = (offset % 3600) / 60;
		printf("UTC offset: %0d:%02d\n", hours, abs(minutes));
	} else {
		printf("localtime returned NULL\n");
	}

	return 0;
}
