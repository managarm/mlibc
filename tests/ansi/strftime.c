#include <string.h>
#include <time.h>
#include <assert.h>
#include <locale.h>
#include <stdio.h>

int main() {
	// Date representation depends on locale, here only C is tested.
	// Maybe consider testing more locales?
	if (setlocale (LC_ALL, "C") == NULL) {
		fputs("strftime testcase could not set locale, errors may be expected!", stderr);
	}

	char timebuf[32];
	char result[32] = " 8";
	struct tm tm;
	tm.tm_sec = 0;
	tm.tm_min = 17;
	tm.tm_hour = 17;
	tm.tm_mday = 8;
	tm.tm_mon = 2;
	tm.tm_year = 121;
	tm.tm_wday = 2;
	tm.tm_yday = 39;
	tm.tm_zone = "CET";
	tm.tm_gmtoff = 3600;
	strftime(timebuf, sizeof(timebuf), "%e", &tm);
	assert(!strcmp(timebuf, result));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%x", &tm);
	assert(!strcmp(timebuf, "03/08/21"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%X", &tm);
	assert(!strcmp(timebuf, "17:17:00"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%D", &tm);
	assert(!strcmp(timebuf, "03/08/21"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%OB %Ob", &tm);
	assert(!strcmp(timebuf, "March Mar"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%c", &tm);
	memset(result, 0, sizeof(result));
	strftime(result, sizeof(result), "%a %b %e %H:%M:%S %Y", &tm);
	assert(!strcmp(timebuf, result));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%k %p %P%n", &tm);
	assert(!strcmp(timebuf, "17 PM pm\n"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%a %A", &tm);
	assert(!strcmp(timebuf, "Tue Tuesday"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%b %B %h", &tm);
	assert(!strcmp(timebuf, "Mar March Mar"));

	memset(timebuf, 0, sizeof(timebuf));
	assert(!strftime(timebuf, sizeof(timebuf), "%a %A %a %A %b %B %h", &tm));

	// cross-glibc on aarch64 and m68k returns an empty string for some reason.
#if !defined(USE_CROSS_LIBC)
	memset(timebuf, 0, sizeof(timebuf));
	size_t ret = strftime(timebuf, sizeof(timebuf), "%z", &tm);
	fprintf(stderr, "strftime(%%z) returned %zu with '%.*s'\n", ret, (int) ret, timebuf);
	assert(!strcmp(timebuf, "+0100"));
#endif

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%U", &tm);
	fprintf(stderr, "'%s'\n", timebuf);
	assert(!strcmp(timebuf, "06"));

	char *locale = setlocale(LC_ALL, "de_DE.utf8");
	assert(locale && strlen(locale));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%a %A", &tm);
	assert(!strcmp(timebuf, "Di Dienstag"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%b %B %h", &tm);
	assert(!strcmp(timebuf, "Mär März Mär"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%c", &tm);
	assert(!strcmp(timebuf, "Di 08 Mär 2021 17:17:00 CET"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%p", &tm);
	assert(!strcmp(timebuf, ""));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%r", &tm);
	assert(!strcmp(timebuf, "05:17:00 "));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%x", &tm);
	assert(!strcmp(timebuf, "08.03.2021"));

	memset(timebuf, 0, sizeof(timebuf));
	strftime(timebuf, sizeof(timebuf), "%X", &tm);
	assert(!strcmp(timebuf, "17:17:00"));

	return 0;
}
