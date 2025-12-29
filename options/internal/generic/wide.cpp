#include <mlibc/wide.hpp>

namespace mlibc {

wctrans_t find_wctrans(frg::string_view property, mlibc::localeinfo *l) {
	auto names_map = l->ctype.map_names();
	size_t index = 0;

	while (names_map.size() && !names_map.starts_with("")) {
		size_t end = names_map.find_first('\0');
		size_t name_len = (end != size_t(-1)) ? end : names_map.size();

		auto name = names_map.sub_string(0, name_len);
		if (name == property)
			break;

		if (end != size_t(-1))
			names_map = names_map.sub_string(end + 1, names_map.size() - (end + 1));
		index++;
	}

	return l->ctype.map_offset() + index;
}

} // namespace mlibc
