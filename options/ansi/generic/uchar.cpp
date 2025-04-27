#include <bits/ensure.h>
#include <uchar.h>
#include <wchar.h>

size_t c32rtomb(char *__restrict s, char32_t c32, mbstate_t *__restrict ps) {
	return wcrtomb(s, c32, ps);
}

size_t mbrtoc32(char32_t *__restrict pc32, const char *__restrict pmb, size_t max, mbstate_t *__restrict ps) {
	static mbstate_t internal_state;

	if(!ps)
		ps = &internal_state;

	if(!pmb)
		return mbrtoc32(0, "", 1, ps);

	wchar_t wc;
	size_t ret = mbrtowc(&wc, pmb, max, ps);

	if (ret <= 4 && pc32)
		*pc32 = wc;

	return ret;
}
