#include <stdio.h>
#include <time.h>

extern int daylight;
extern long timezone;
extern char *tzname[2];

int main() {
	tzset();
	printf("%s %s %ld %d\n", tzname[0], tzname[1], timezone, daylight);
	return 0;
}
