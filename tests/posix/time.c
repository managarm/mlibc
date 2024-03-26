#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 1024

int main() {
	struct tm tm = {0};
	char buf[BUF_SIZE];

	char *a = strptime("%", "%%", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(strftime(buf, BUF_SIZE, "%%", &tm) == 1);
	assert(!strcmp(buf, "%"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("1991-11-21", "%F", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mday == 21);
	assert(tm.tm_mon == 10);
	assert(tm.tm_wday == 4);
	assert(tm.tm_yday == 324);
	assert(strftime(buf, BUF_SIZE, "%F", &tm) == 10);
	assert(!strcmp(buf, "1991-11-21"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("10/19/91", "%D", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mday == 19);
	assert(tm.tm_mon  == 9);
	assert(tm.tm_year == 91);
	assert(tm.tm_wday == 6);
	assert(tm.tm_yday == 291);
	assert(strftime(buf, BUF_SIZE, "%D", &tm) == 8);
	assert(!strcmp(buf, "10/19/91"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("15:23", "%R", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_min  == 23);
	assert(tm.tm_hour == 15);
	assert(strftime(buf, BUF_SIZE, "%R", &tm) == 5);
	assert(!strcmp(buf, "15:23"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("17:12:56", "%T", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_sec  == 56);
	assert(tm.tm_min  == 12);
	assert(tm.tm_hour == 17);
	assert(strftime(buf, BUF_SIZE, "%T", &tm) == 8);
	assert(!strcmp(buf, "17:12:56"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("10", "%m", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_yday == 272);
	assert(strftime(buf, BUF_SIZE, "%m", &tm) == 2);
	assert(!strcmp(buf, "10"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("14 83", "%C %y", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_year == -417);
	assert(strftime(buf, BUF_SIZE, "%C %y", &tm) == 5);
	assert(!strcmp(buf, "14 83"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("32 16", "%y %C", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_year == -268);
	assert(tm.tm_wday == 3);
	assert(strftime(buf, BUF_SIZE, "%y %C", &tm) == 5);
	assert(!strcmp(buf, "32 16"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("12", "%C", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_year == -700);
	assert(tm.tm_wday == 5);
	assert(strftime(buf, BUF_SIZE, "%C", &tm) == 2);
	assert(!strcmp(buf, "12"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("1683-9-23", "%F", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mday == 23);
	assert(tm.tm_mon  == 8);
	assert(tm.tm_year == -217);
	assert(tm.tm_wday == 4);
	assert(tm.tm_yday == 265);
	assert(strftime(buf, BUF_SIZE, "%F", &tm) == 10);
	assert(!strcmp(buf, "1683-09-23"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("14 53", "%H%t%S", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_sec  == 53);
	assert(tm.tm_hour == 14);
	assert(strftime(buf, BUF_SIZE, "%H%t%S", &tm) == 5);
	assert(!strcmp(buf, "14	53"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("24", "%H", &tm);
	assert(a == NULL);
	memset(&tm, 0, sizeof(tm));

	a = strptime("0", "%I", &tm);
	assert(a == NULL);
	memset(&tm, 0, sizeof(tm));

	setlocale(LC_TIME, "en_US.UTF-8");
	a = strptime("10 21 PM", "%I %M %p", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_hour == 22);
	assert(tm.tm_min  == 21);
	assert(strftime(buf, BUF_SIZE, "%I %M %p", &tm) == 8);
	assert(!strcmp(buf, "10 21 PM"));
	memset(&tm, 0, sizeof(tm));

	tm.tm_min = 23;
	assert(strftime(buf, BUF_SIZE, "%I %M", &tm) == 5);
	assert(!strcmp(buf, "12 23"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("January", "%h", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mon == 0);
	assert(strftime(buf, BUF_SIZE, "%h %b", &tm) == 7);
	assert(!strcmp(buf, "Jan Jan"));
	assert(strftime(buf, BUF_SIZE, "%B", &tm) == 7);
	assert(!strcmp(buf, "January"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("2", "%j", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_yday == 1);
	assert(strftime(buf, BUF_SIZE, "%j", &tm) == 3);
	assert(!strcmp(buf, "002"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("Wednesday", "%A", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_wday == 3);
	assert(strftime(buf, BUF_SIZE, "%A", &tm) == 9);
	assert(!strcmp(buf, "Wednesday"));
	memset(&tm, 0, sizeof(tm));

	a = strptime("11:51:13 PM", "%r", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_hour == 23);
	assert(tm.tm_min == 51);
	assert(tm.tm_sec == 13);
	assert(strftime(buf, BUF_SIZE, "%r", &tm) == 11);
	assert(!strcmp(buf, "11:51:13 PM"));
	memset(&tm, 0, sizeof(tm));

	tm.tm_hour = 0;
	tm.tm_min = 51;
	tm.tm_sec = 13;
	assert(strftime(buf, BUF_SIZE, "%r", &tm) == 11);
	assert(!strcmp(buf, "12:51:13 AM"));
	memset(&tm, 0, sizeof(tm));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(strftime(buf, BUF_SIZE, "%", &tm) == 1);
	fprintf(stderr, "%s\n", buf);
	assert(!strcmp(buf, "%"));
	memset(&tm, 0, sizeof(tm));
#pragma GCC diagnostic pop

	return 0;
}
