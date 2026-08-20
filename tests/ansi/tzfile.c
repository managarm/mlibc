#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern int daylight;
extern long timezone;
extern char *tzname[2];

static void put32(uint8_t *data, size_t *pos, uint32_t value) {
	data[(*pos)++] = value >> 24;
	data[(*pos)++] = value >> 16;
	data[(*pos)++] = value >> 8;
	data[(*pos)++] = value;
}

static void put64(uint8_t *data, size_t *pos, uint64_t value) {
	put32(data, pos, value >> 32);
	put32(data, pos, value);
}

static void put_header(uint8_t *data, size_t *pos, uint8_t version, uint32_t timecnt,
		uint32_t typecnt, uint32_t charcnt) {
	data[(*pos)++] = 'T';
	data[(*pos)++] = 'Z';
	data[(*pos)++] = 'i';
	data[(*pos)++] = 'f';
	data[(*pos)++] = version;
	*pos += 15;
	put32(data, pos, 0);
	put32(data, pos, 0);
	put32(data, pos, 0);
	put32(data, pos, timecnt);
	put32(data, pos, typecnt);
	put32(data, pos, charcnt);
}

static void put_type(uint8_t *data, size_t *pos, int32_t offset, uint8_t isdst, uint8_t abbrind) {
	put32(data, pos, (uint32_t)offset);
	data[(*pos)++] = isdst;
	data[(*pos)++] = abbrind;
}

static void write_tzfile(const uint8_t *data, size_t size, char *path) {
	strcpy(path, "/tmp/mlibc-tzfile-XXXXXX");
	int fd = mkstemp(path);
	assert(fd >= 0);
	assert(write(fd, data, size) == (ssize_t)size);
	assert(close(fd) == 0);
}

static void check_localtime(time_t time, int hour, int min, int sec, long offset, int isdst,
		const char *zone) {
	struct tm tm;
	assert(localtime_r(&time, &tm) == &tm);
	assert(tm.tm_hour == hour);
	assert(tm.tm_min == min);
	assert(tm.tm_sec == sec);
	assert(tm.tm_gmtoff == offset);
	assert(tm.tm_isdst == isdst);
	assert(!strcmp(tm.tm_zone, zone));
}

int main() {
	uint8_t transitions[156] = {};
	size_t pos = 0;
	put_header(transitions, &pos, '2', 0, 1, 4);
	put_type(transitions, &pos, 0, 0, 0);
	memcpy(transitions + pos, "OLD", 4);
	pos += 4;
	put_header(transitions, &pos, '2', 2, 2, 8);
	put64(transitions, &pos, 0);
	put64(transitions, &pos, 86400);
	transitions[pos++] = 1;
	transitions[pos++] = 0;
	put_type(transitions, &pos, 3600, 0, 0);
	put_type(transitions, &pos, 7200, 1, 4);
	memcpy(transitions + pos, "STD\0DST", 8);
	pos += 8;
	// The v2/v3 footer keeps the post-transition behavior valid for host libc.
	memcpy(transitions + pos, "\nSTD-1DST-2,0/0,1/0\n", 20);
	pos += 20;
	assert(pos == sizeof(transitions));

	char path[sizeof("/tmp/mlibc-tzfile-XXXXXX")];
	write_tzfile(transitions, sizeof(transitions), path);
	// Select the generated TZif v2 file: +01 STD, +02 DST on January 1.
	assert(setenv("TZ", path, 1) == 0);
	tzset();
	assert(!strcmp(tzname[0], "STD"));
	assert(!strcmp(tzname[1], "DST"));
	assert(timezone == -3600);
	assert(daylight == 1);
	check_localtime(-1, 0, 59, 59, 3600, 0, "STD");
	check_localtime(0, 2, 0, 0, 7200, 1, "DST");
	check_localtime(86399, 1, 59, 59, 7200, 1, "DST");
	check_localtime(86400, 1, 0, 0, 3600, 0, "STD");
	check_localtime(86401, 1, 0, 1, 3600, 0, "STD");
	assert(!strcmp(tzname[0], "STD"));
	assert(!strcmp(tzname[1], "DST"));
	assert(unlink(path) == 0);

	uint8_t fixed[118] = {};
	pos = 0;
	put_header(fixed, &pos, '2', 0, 1, 4);
	put_type(fixed, &pos, 0, 0, 0);
	memcpy(fixed + pos, "OLD", 4);
	pos += 4;
	put_header(fixed, &pos, '2', 0, 1, 4);
	put_type(fixed, &pos, 5400, 0, 0);
	memcpy(fixed + pos, "FIX", 4);
	pos += 4;
	memcpy(fixed + pos, "\nFIX-1:30\n", 10);
	pos += 10;
	assert(pos == sizeof(fixed));

	write_tzfile(fixed, sizeof(fixed), path);
	// Select the generated fixed-offset TZif v2 file: FIX is UTC+01:30.
	assert(setenv("TZ", path, 1) == 0);
	tzset();
	assert(!strcmp(tzname[0], "FIX"));
	assert(timezone == -5400);
	assert(daylight == 0);
	check_localtime(0, 1, 30, 0, 5400, 0, "FIX");
	assert(unlink(path) == 0);

	return 0;
}
