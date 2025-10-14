#ifndef MLIBC_LOCALE
#define MLIBC_LOCALE

#include <array>
#include <bits/nl_item.h>
#include <frg/span.hpp>

namespace mlibc {

extern frg::array<uint8_t, 768> cLocaleCtypeClass;
extern frg::array<uint8_t, 1024> cLocaleCtypeClass32;
extern frg::array<uint32_t, 384> cLocaleCtypeToUpper;
extern frg::array<uint32_t, 384> cLocaleCtypeToLower;
extern frg::array<uint32_t, 10> cLocaleClassUpper;
extern frg::array<uint32_t, 10> cLocaleClassLower;
extern frg::array<uint32_t, 10> cLocaleClassAlpha;
extern frg::array<uint32_t, 9> cLocaleClassDigit;
extern frg::array<uint32_t, 11> cLocaleClassXdigit;
extern frg::array<uint32_t, 9> cLocaleClassSpace;
extern frg::array<uint32_t, 11> cLocaleClassPrint;
extern frg::array<uint32_t, 11> cLocaleClassGraph;
extern frg::array<uint32_t, 9> cLocaleClassBlank;
extern frg::array<uint32_t, 11> cLocaleClassCntrl;
extern frg::array<uint32_t, 11> cLocaleClassPunct;
extern frg::array<uint32_t, 11> cLocaleClassAlnum;
extern frg::array<uint32_t, 42> cLocaleMapToUpper;
extern frg::array<uint32_t, 42> cLocaleMapToLower;

char *nl_langinfo(nl_item item);

} // namespace mlibc

#endif // MLIBC_LOCALE
