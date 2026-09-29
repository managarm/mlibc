#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <locale.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 1024

#define ROUNDTRIP(format, member, min, max) \
	for (int i = min; i < max; i++) {\
		memset(&tm, 0, sizeof(tm));\
		tm.member = i;\
		int f = strftime(buf, BUF_SIZE, format, &tm);\
		assert(f);\
		memset(&tm, 0, sizeof(tm));\
		char *p = strptime(buf, format, &tm);\
		assert(p != NULL);\
		assert(tm.member == i);\
	}

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

	a = strptime("Novembe", "%B", &tm);
	assert(a != NULL);
	assert(*a == 'e');
	assert(tm.tm_mon == 10);
	memset(&tm, 0, sizeof(tm));

	a = strptime("Marc", "%B", &tm);
	assert(a != NULL);
	assert(*a == 'c');
	assert(tm.tm_mon == 2);
	memset(&tm, 0, sizeof(tm));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
	assert(strftime(buf, BUF_SIZE, "%", &tm) == 1);
	assert(!strcmp(buf, "%"));
	memset(&tm, 0, sizeof(tm));
#pragma GCC diagnostic pop

	char *l = setlocale(LC_ALL, "ja_JP.utf8");
	assert(l && strlen(l));

	ROUNDTRIP("%a", tm_wday, 0, 6);
	ROUNDTRIP("%A", tm_wday, 0, 6);
	ROUNDTRIP("%b", tm_mon, 0, 12);
	ROUNDTRIP("%Ob", tm_mon, 0, 12);
	ROUNDTRIP("%B", tm_mon, 0, 12);
	ROUNDTRIP("%OB", tm_mon, 0, 12);
	ROUNDTRIP("%e", tm_mday, 1, 32);
	ROUNDTRIP("%h", tm_mon, 0, 12);
	ROUNDTRIP("%H", tm_hour, 0, 24);
	ROUNDTRIP("%I %p", tm_hour, 0, 12);
	ROUNDTRIP("%j", tm_yday, 1, 366);
	ROUNDTRIP("%m", tm_mon, 1, 12);
	ROUNDTRIP("%M", tm_min, 0, 60);
	ROUNDTRIP("%S", tm_sec, 0, 61);
	ROUNDTRIP("%w", tm_wday, 0, 7);
	ROUNDTRIP("%Y", tm_year, 1900, 2100);

	memset(&tm, 0, sizeof(tm));
	tm.tm_mon = 6;
	tm.tm_min = 1;
	tm.tm_hour = 16;
	tm.tm_year = 1999;
	tm.tm_mday = 2;
	tm.tm_wday = 0;
	tm.tm_yday = 48;

	int f = strftime(buf, BUF_SIZE, "%b", &tm);
	assert(!strcmp(buf, " 7月"));
	assert(f == 5);

	f = strftime(buf, BUF_SIZE, "%Ob", &tm);
	assert(!strcmp(buf, " 7月"));
	assert(f == 5);
	f = strftime(buf, BUF_SIZE, "%OB", &tm);
	assert(!strcmp(buf, "7月"));
	assert(f == 4);
	f = strftime(buf, BUF_SIZE, "%Od", &tm);
	assert(!strcmp(buf, "二"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%Oe", &tm);
	assert(!strcmp(buf, "二"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%OH", &tm);
	assert(!strcmp(buf, "十六"));
	assert(f == 6);
	f = strftime(buf, BUF_SIZE, "%OI", &tm);
	assert(!strcmp(buf, "四"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%Om", &tm);
	assert(!strcmp(buf, "七"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%OM", &tm);
	assert(!strcmp(buf, "一"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%OS", &tm);
	assert(!strcmp(buf, "〇"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%Ou", &tm);
	assert(!strcmp(buf, "七"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%OU", &tm);
	assert(!strcmp(buf, "七"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%OV", &tm);
	assert(!strcmp(buf, "七"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%Ow", &tm);
	assert(!strcmp(buf, "〇"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%OW", &tm);
	assert(!strcmp(buf, "七"));
	assert(f == 3);
	f = strftime(buf, BUF_SIZE, "%Oy", &tm);
	assert(!strcmp(buf, "九十九"));
	assert(f == 9);

	a = strptime("二十一", "%Od", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mday == 21);
	memset(&tm, 0, sizeof(tm));

	a = strptime("五", "%Oe", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mday == 5);
	memset(&tm, 0, sizeof(tm));

	a = strptime("十八", "%OH", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_hour == 18);
	memset(&tm, 0, sizeof(tm));

	a = strptime("六", "%OI", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_hour == 6);
	memset(&tm, 0, sizeof(tm));

	a = strptime("八", "%Om", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_mon == 7);
	memset(&tm, 0, sizeof(tm));

	a = strptime("四十五", "%OM", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_min == 45);
	memset(&tm, 0, sizeof(tm));

	a = strptime("五十九", "%OS", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_sec == 59);
	memset(&tm, 0, sizeof(tm));

	a = strptime("三", "%Ow", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_wday == 3);
	memset(&tm, 0, sizeof(tm));

	a = strptime("三", "%OW", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	memset(&tm, 0, sizeof(tm));

	a = strptime("二十四", "%Oy", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_year == 124);
	memset(&tm, 0, sizeof(tm));

	a = strptime("九十五", "%Oy", &tm);
	assert(a != NULL);
	assert(*a == '\0');
	assert(tm.tm_year == 95);
	memset(&tm, 0, sizeof(tm));

	return 0;
}
