#include <iconv.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

size_t iconv(iconv_t cd, char **__restrict inbuf, size_t *__restrict inbytesleft, char **__restrict outbuf, size_t *__restrict outbytesleft) {
	(void)inbytesleft;
	(void)outbytesleft;

	mlibc::infoLogger() << "iconv() is unimplemented!" << frg::endlog;
	if(cd == (iconv_t)1) { // UTF-8 to UTF-8
		mlibc::infoLogger() << "iconv() from and to are the same, memcpy it is" << frg::endlog;
		memcpy(inbuf, outbuf, sizeof(inbuf));
		return sizeof(outbuf);
	}
	__ensure(!"iconv() not implemented");
	__builtin_unreachable();
}

int iconv_close(iconv_t) {
	return 0;
}

iconv_t iconv_open(const char *tocode, const char *fromcode) {
	mlibc::infoLogger() << "iconv_open() is unimplemented! args: " << tocode << " and: " << fromcode << frg::endlog;
	if(!strcmp(tocode, "UTF-8") && !strcmp(fromcode, "UTF-8")) {
		mlibc::infoLogger() << "iconv_open() with UTF-8 on both is a no-op!" << frg::endlog;
		iconv_t cd = (iconv_t)1;
		return cd;
	}
	__ensure(!"iconv_open() not implemented");
	__builtin_unreachable();
}


