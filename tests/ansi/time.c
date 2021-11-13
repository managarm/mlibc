#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    struct tm tm = {0};

    char *a = strptime("%", "%%", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    memset(&tm, 0, sizeof(tm));

    a = strptime("1991-11-21", "%F", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_mday == 21);
    assert(tm.tm_mon == 10);
    assert(tm.tm_wday == 4);
    assert(tm.tm_yday == 324);
    memset(&tm, 0, sizeof(tm));

    a = strptime("10/19/91", "%D", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_mday == 19);
    assert(tm.tm_mon  == 9);
    assert(tm.tm_year == 91);
    assert(tm.tm_wday == 6);
    assert(tm.tm_yday == 291);
    memset(&tm, 0, sizeof(tm));

    a = strptime("15:23", "%R", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_min  == 23);
    assert(tm.tm_hour == 15);
    memset(&tm, 0, sizeof(tm));

    a = strptime("17:12:56", "%T", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_sec  == 56);
    assert(tm.tm_min  == 12);
    assert(tm.tm_hour == 17);
    memset(&tm, 0, sizeof(tm));

    a = strptime("10", "%m", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_yday == 272);
    memset(&tm, 0, sizeof(tm));

    a = strptime("14 83", "%C %y", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_year == -417);
    memset(&tm, 0, sizeof(tm));

    a = strptime("32 16", "%y %C", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_year == -268);
    assert(tm.tm_wday == 3);
    memset(&tm, 0, sizeof(tm));

    a = strptime("12", "%C", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_year == -700);
    assert(tm.tm_wday == 5);
    memset(&tm, 0, sizeof(tm));

    a = strptime("1683-9-23", "%F", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_mday == 23);
    assert(tm.tm_mon  == 8);
    assert(tm.tm_year == -217);
    assert(tm.tm_wday == 4);
    assert(tm.tm_yday == 265);
    memset(&tm, 0, sizeof(tm));

    a = strptime("14 53", "%H%t%S", &tm);
    assert(a != NULL);
    assert(*a == '\0');
    assert(tm.tm_sec  == 53);
    assert(tm.tm_hour == 14);
    memset(&tm, 0, sizeof(tm));
}
