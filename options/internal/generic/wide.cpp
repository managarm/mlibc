#include <mlibc/wide.hpp>

namespace mlibc {

wctrans_t find_wctrans(frg::string_view property, mlibc::localeinfo *l) {
	auto names_map = l->ctype.map_names();
	size_t index = 0;

	while (names_map.size() && names_map[0] != '\0') {
		size_t end = names_map.find_first('\0');
		size_t name_len = (end != size_t(-1)) ? end : names_map.size();

		auto name = names_map.sub_string(0, name_len);
		if (name == property)
			return l->ctype.map_offset() + index;

		if (end != size_t(-1))
			names_map = names_map.sub_string(end + 1, names_map.size() - (end + 1));
		index++;
	}

	return 0;
}

wchar_t *wcpncpy(wchar_t *__restrict ws1, const wchar_t *__restrict ws2, size_t n) {
	for (; n--; ws1++, ws2++) {
		if (!(*ws1 = *ws2)) {
			wchar_t *ret = ws1;
			while (n--)
				*++ws1 = L'\0';
			return ret;
		}
	}
	return ws1;
}

} // namespace mlibc
