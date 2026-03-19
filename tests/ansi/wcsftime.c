#include <assert.h>
#include <time.h>
#include <wchar.h>

int main(void) {
	struct tm tm = {
	    .tm_sec = 07,
	    .tm_min = 45,
	    .tm_hour = 20,
	    .tm_mday = 19,
	    .tm_mon = 2,
	    .tm_year = 126,
	    .tm_wday = 4,
	    .tm_yday = 77,
	};
	wchar_t buf[64];
	size_t length = wcsftime(buf, sizeof(buf), L"%Y-%m-%d %H:%M:%S", &tm);
	assert(length == 19);
	assert(!wcscmp(buf, L"2026-03-19 20:45:07"));

	return 0;
}
