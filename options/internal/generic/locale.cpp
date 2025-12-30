#include <frg/allocation.hpp>
#include <frg/hash_map.hpp>
#include <frg/scope_exit.hpp>
#include <frg/vector.hpp>
#include <locale.h>
#include <smarter.hpp>
#include <mlibc-config.h>
#include <mlibc/ctype.hpp>
#include <mlibc/file-window.hpp>
#include <mlibc/global-config.hpp>
#include <mlibc/locale-archive-parsing.hpp>
#include <mlibc/locale.hpp>
#include <stdlib.h>

namespace {

mlibc::localeinfo cLocale{};
mlibc::localeinfo startingLocale{};

thread_local mlibc::localeinfo *current_locale = nullptr;
mlibc::localeinfo *current_global_locale = &startingLocale;

// handle to the opened locale-archive file, if any
smarter::shared_ptr<file_window> localeArchive;

// map of (locale name, mapping)
frg::hash_map<
	frg::string<MemoryAllocator>,
	smarter::shared_ptr<file_window>,
	frg::hash<frg::string<MemoryAllocator>>,
	MemoryAllocator
> localeFiles{frg::hash<frg::string<MemoryAllocator>>(), getAllocator()};

// value of LOCPATH environment variable, if set
frg::string<MemoryAllocator> cachedLocpath{getAllocator()};
// list of paths to inspect for searching locale (non-archive) files
frg::vector<frg::string<MemoryAllocator>, MemoryAllocator> locpaths{getAllocator()};

std::array<const char *, 13> lcNames{
	"LC_CTYPE",
	"LC_NUMERIC",
	"LC_TIME",
	"LC_COLLATE",
	"LC_MONETARY",
	"LC_MESSAGES",
	"LC_ALL",
	"LC_PAPER",
	"LC_NAME",
	"LC_ADDRESS",
	"LC_TELEPHONE",
	"LC_MEASUREMENT",
	"LC_IDENTIFICATION",
};

using DecomposedLocale = std::tuple<
	std::optional<frg::string_view>, // language
	std::optional<frg::string_view>, // territory
	std::optional<frg::string_view>, // codeset
	std::optional<frg::string_view> // modifier
>;

frg::string<MemoryAllocator> normalizeCodeset(frg::string_view codeset) {
	bool only_digit = true;

	for (size_t i = 0; i < codeset.size(); i++) {
		if (only_digit && !mlibc::isdigit_l(codeset[i], &cLocale))
			only_digit = false;
	}

	frg::string<MemoryAllocator> normalized{getAllocator()};

	if (only_digit)
		normalized += "iso";

	for(size_t i = 0; i < codeset.size(); i++) {
		if (mlibc::isalpha_l(codeset[i], &cLocale))
			normalized.push_back(tolower_l(codeset[i], &cLocale));
		else if (mlibc::isdigit_l(codeset[i], &cLocale))
			normalized.push_back(codeset[i]);
	}

	return normalized;
}

// given a locale name string, break it into its constituent parts
DecomposedLocale parseLocaleName(frg::string_view name) {
	std::optional<frg::string_view> language;
	std::optional<frg::string_view> territory;
	std::optional<frg::string_view> codeset;
	std::optional<frg::string_view> modifier;

	size_t offset = 0;
	size_t territoryEnd = size_t(-1);

	auto languageLen = name.find_first_of({"_.@", 3});
	if (languageLen != size_t(-1)) {
		language = name.sub_string(0, languageLen);
		offset = languageLen + 1;
	} else {
		language = name;
		return {language, territory, codeset, modifier};
	}

	if (name[languageLen] == '_') {
		territoryEnd = name.find_first_of({".@", 2}, offset);
		if (territoryEnd != size_t(-1)) {
			if (territoryEnd > offset)
				territory = name.sub_string(offset, territoryEnd - offset);
			offset = territoryEnd + 1;
		} else {
			if (name.size() > offset)
				territory = name.sub_string(offset, name.size() - offset);
			return {language, territory, codeset, modifier};
		}
	}

	if (name[frg::max(languageLen, (territoryEnd == size_t(-1) ? 0 : territoryEnd))] == '.') {
		auto codesetEnd = name.find_first('@', offset);
		if (codesetEnd != size_t(-1)) {
			if (codesetEnd > offset)
				codeset = name.sub_string(offset, codesetEnd - offset);

			offset = codesetEnd + 1;
		} else {
			if (name.size() > offset)
				codeset = name.sub_string(offset, name.size() - offset);

			return {language, territory, codeset, modifier};
		}
	}

	if (name.size() > offset)
		modifier = name.sub_string(offset, name.size() - offset);

	return {language, territory, codeset, modifier};
}

// Given a DecomposedLocale, build a prioritized list of locale names to check
frg::vector<frg::string<MemoryAllocator>, MemoryAllocator> buildLocaleList(DecomposedLocale loc) {
	auto &[lang, territory, codeset, modifier] = loc;
	__ensure(lang);

	auto normCodeset = [&] {
		if (codeset)
			return normalizeCodeset(*codeset);
		else
			return frg::string{getAllocator()};
	}();
	frg::vector<frg::string<MemoryAllocator>, MemoryAllocator> ret{getAllocator()};

	// the ordering of the components is apparently:
	// lang > TERRITORY > normcodeset > codeset > modifier
	// codeset is skipped if it equals normcodeset

	// lang_TERRITORY.codeset@modifier
	if (territory && codeset && codeset != normCodeset && modifier) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '_';
		str += *territory;
		str += '.';
		str += *codeset;
		str += '@';
		str += *modifier;
		ret.push_back(std::move(str));
	}
	// lang_TERRITORY.normcodeset@modifier
	if (territory && !normCodeset.empty() && modifier) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '_';
		str += *territory;
		str += '.';
		str += normCodeset;
		str += '@';
		str += *modifier;
		ret.push_back(std::move(str));
	}
	// lang_TERRITORY@modifier
	if (territory && modifier) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '_';
		str += *territory;
		str += '@';
		str += *modifier;
		ret.push_back(std::move(str));
	}
	// lang_TERRITORY.codeset
	if (territory && codeset && codeset != normCodeset) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '_';
		str += *territory;
		str += '.';
		str += *codeset;
		ret.push_back(std::move(str));
	}
	// lang_TERRITORY.normcodeset
	if (territory && !normCodeset.empty()) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '_';
		str += *territory;
		str += '.';
		str += normCodeset;
		ret.push_back(std::move(str));
	}
	// lang_TERRITORY
	if (territory) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '_';
		str += *territory;
		ret.push_back(std::move(str));
	}
	// lang.codeset@modifier
	if (modifier && codeset && codeset != normCodeset) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '.';
		str += *codeset;
		str += '@';
		str += *modifier;
		ret.push_back(std::move(str));
	}
	// lang.normcodeset@modifier
	if (modifier && !normCodeset.empty()) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '.';
		str += normCodeset;
		str += '@';
		str += *modifier;
		ret.push_back(std::move(str));
	}
	// lang@modifier
	if (modifier) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '@';
		str += *modifier;
		ret.push_back(std::move(str));
	}
	// lang.codeset
	if (codeset && codeset != normCodeset) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '.';
		str += *codeset;
		ret.push_back(std::move(str));
	}
	// lang.normcodeset
	if (!normCodeset.empty()) {
		frg::string<MemoryAllocator> str{*lang, getAllocator()};
		str += '.';
		str += normCodeset;
		ret.push_back(std::move(str));
	}
	// lang
	ret.push_back(frg::string<MemoryAllocator>{*lang, getAllocator()});

	if (mlibc::globalConfig().debugLocale) {
		mlibc::infoLogger() << "mlibc: locale name list for '" << ret[0] << "':" << frg::endlog;
		for (auto e : ret)
			mlibc::infoLogger() << frg::fmt("\t'{}'", e) << frg::endlog;
	}

	return ret;
}

// Parse a composite locale string into its constituents, if applicable.
std::optional<std::array<frg::string_view, LC_IDENTIFICATION + 1>> parseCompositeLocaleName(frg::string_view str) {
	size_t offset = 0;

	std::array<frg::string_view, LC_IDENTIFICATION + 1> parsed;

	for (size_t i = LC_CTYPE; i <= LC_IDENTIFICATION; i++) {
		if (i == LC_ALL)
			continue;

		auto eq = str.find_first('=', offset);
		if (eq == size_t(-1))
			eq = str.size();
		auto catName = str.sub_string(offset, eq - offset);
		if (catName != lcNames[i])
			return std::nullopt;
		auto semic = str.find_first(';', eq + 1);
		if (semic == size_t(-1))
			semic = str.size();
		auto catValue = str.sub_string(eq + 1, semic - (eq + 1));
		if (!catValue.size())
			return std::nullopt;

		parsed[i] = catValue;

		if (semic == str.size()) {
			if (i == LC_IDENTIFICATION) {
				return parsed;
			} else {
				break;
			}
		}

		offset = semic + 1;
	}

	return std::nullopt;
}

// open the locale-archive database, if it exists
void openLocaleDatabase() {
#if !MLIBC_MAP_FILE_WINDOWS
	return;
#endif

	if (locpaths.empty()) {
		auto locpath = getenv("LOCPATH");
		if (locpath && strlen(locpath)) {
			cachedLocpath = {locpath, getAllocator()};
			size_t off = 0;
			size_t end = 0;
			while(end != size_t(-1)) {
				auto view = frg::string_view{cachedLocpath}.sub_string(off, cachedLocpath.size() - off);
				end = view.find_first(':');
				locpaths.push_back(
					frg::string<MemoryAllocator>{
						view.sub_string(0, (end == size_t(-1)) ? view.size() : end),
						getAllocator()
					}
				);
			}
		}
		locpaths.push_back(frg::string<MemoryAllocator>{"/usr/lib/locale", getAllocator()});
		locpaths.push_back(frg::string<MemoryAllocator>{"/usr/share/i18n/locales", getAllocator()});
	}

	if (!cachedLocpath.empty())
		return;

	if (localeArchive)
		return;

	struct stat info;
	if (mlibc::sys_stat(mlibc::fsfd_target::path, -1, "/usr/lib/locale/locale-archive", 0, &info))
		return;

	auto window = smarter::allocate_shared<file_window>(getAllocator(), "/usr/lib/locale/locale-archive");
	LocaleArchive::Header *header = static_cast<LocaleArchive::Header *>(window->get());
	if (header->magic != LocaleArchive::HEADER_MAGIC)
		return;

	localeArchive = std::move(window);
}

frg::optional<off_t> findLocaleRecord(frg::string_view  name) {
	__ensure(localeArchive);

	auto header = static_cast<LocaleArchive::Header *>(localeArchive->get());
	if (header->namehash_size <= 2)
		return frg::null_opt;
	auto namehashtab = reinterpret_cast<LocaleArchive::NameHashEntry *>((uint8_t *) localeArchive->get() + header->namehash_offset);

	auto parsedName = parseLocaleName(name);
	auto list = buildLocaleList(parsedName);

	for (auto e : list) {
		auto hash = nameHashVal<uint32_t>(e.data(), e.size());
		auto idx = hash % header->namehash_size;
		auto incr = 1 + hash % (header->namehash_size - 2);

		while(true) {
			if (namehashtab[idx].name_offset == 0)
				break;

			auto current_entry_name = (char *) localeArchive->get() + namehashtab[idx].name_offset;

			if (namehashtab[idx].hashval == hash && !strcmp(e.data(), current_entry_name))
				return namehashtab[idx].locrec_offset ? frg::optional<off_t>{namehashtab[idx].locrec_offset} : frg::null_opt;

			idx += incr;
			if (idx >= header->namehash_size)
				idx -= header->namehash_size;
		}
	}

	return frg::null_opt;
}

bool parseCategoryInfo(int category, frg::string_view name, frg::span<const uint8_t> rec, mlibc::localeinfo *out) {
	uint32_t magic;
	memcpy(&magic, rec.data(), sizeof(magic));
	if (magic != categoryMagic(category))
		return false;

	uint32_t elements;
	memcpy(&elements, rec.data() + 4, sizeof(elements));
	frg::span<const uint32_t> offsets{reinterpret_cast<const uint32_t *>(rec.data() + 8), elements};

	switch(category) {
		case LC_CTYPE: {
			parse_category_array(out->ctype.members, ctype_parser, rec, offsets);
			out->ctype.data = rec;
			out->ctype.offsets = offsets;
			out->ctype.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_NUMERIC: {
			parse_category_array(out->numeric.members, numeric_parser, rec, offsets);
			out->numeric.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_TIME: {
			parse_category_array(out->time.members, time_parser, rec, offsets);
			out->time.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_COLLATE:
			parse_category_array(out->collate.members, collate_parser, rec, offsets);
			out->collate.localeName = frg::string{getAllocator(), name};
			break;
		case LC_MONETARY: {
			parse_category_array(out->monetary.members, monetary_parser, rec, offsets);
			out->monetary.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_MESSAGES: {
			parse_category_array(out->messages.members, messages_parser, rec, offsets);
			out->messages.localeName = frg::string{getAllocator(), name};
			break;
		}
		// skip LC_ALL
		case LC_PAPER: {
			parse_category_array(out->paper.members, paper_parser, rec, offsets);
			out->paper.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_NAME: {
			parse_category_array(out->name.members, name_parser, rec, offsets);
			out->name.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_ADDRESS: {
			parse_category_array(out->address.members, address_parser, rec, offsets);
			out->address.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_TELEPHONE: {
			parse_category_array(out->telephone.members, telephone_parser, rec, offsets);
			out->telephone.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_MEASUREMENT: {
			parse_category_array(out->measurement.members, measurement_parser, rec, offsets);
			out->measurement.localeName = frg::string{getAllocator(), name};
			break;
		}
		case LC_IDENTIFICATION: {
			parse_category_array(out->identification.members, identification_parser, rec, offsets);
			out->identification.localeName = frg::string{getAllocator(), name};
			break;
		}
		default:
			mlibc::infoLogger() << "mlibc: category " << lcNames[category] << " unknown!" << frg::endlog;
			return false;
	}

	return true;
}

bool findLocaleFileRecord(int category, frg::string_view name, mlibc::localeinfo *out) {
	auto parsedName = parseLocaleName(name);
	auto list = buildLocaleList(parsedName);

	for (auto lp : locpaths) {
		for (auto locale : list) {
			frg::string<MemoryAllocator> path{lp, getAllocator()};
			path += '/';
			path += locale;
			path += '/';
			if (category == LC_MESSAGES)
				path += "LC_MESSAGES/SYS_LC_MESSAGES";
			else
				path += lcNames[category];

			auto e = localeFiles.find(path);
			if (e != localeFiles.end()) {
				auto &[localeName, window] = *e;
				if (parseCategoryInfo(category, locale, {static_cast<const uint8_t *>(window->get()), window->size()}, out))
					return true;
			} else {
				struct stat info;
				if (mlibc::sys_stat(mlibc::fsfd_target::path, -1, path.data(), 0, &info))
					continue;

				auto window = smarter::allocate_shared<file_window>(getAllocator(), path.data());
				localeFiles.insert(path, window);

				if (parseCategoryInfo(category, locale, {static_cast<const uint8_t *>(window->get()), window->size()}, out)) {
					return true;
				}
			}
		}
	}

	return false;
}

bool findLocaleArchiveRecord(int category, frg::string_view name, mlibc::localeinfo *out) {
	if (!localeArchive)
		return false;

	__ensure(category != LC_ALL);

	auto recordOff = findLocaleRecord(name);
	if (!recordOff) {
		if (mlibc::globalConfig().debugLocale)
			mlibc::infoLogger() << "mlibc: Locale '" << name
				<< "' not found in locale database" << frg::endlog;
		return false;
	}

	auto locrec = reinterpret_cast<LocaleArchive::LocaleRecord *>((uint8_t *) localeArchive->get() + *recordOff);
	if (locrec->record[category].offset == 0 || locrec->record[category].len == 0)
		return false;

	const uint8_t *record = reinterpret_cast<const uint8_t *>(localeArchive->get()) + locrec->record[category].offset;
	frg::span<const uint8_t> rec{record, locrec->record[category].len};

	return parseCategoryInfo(category, name, rec, out);
}

} // namespace

namespace mlibc {

frg::string<MemoryAllocator> &localeinfo::getCategoryLocaleName(int category) {
	switch (category) {
		case LC_CTYPE: return ctype.localeName;
		case LC_NUMERIC: return numeric.localeName;
		case LC_TIME: return time.localeName;
		case LC_COLLATE: return collate.localeName;
		case LC_MONETARY: return monetary.localeName;
		case LC_MESSAGES: return messages.localeName;
		case LC_PAPER: return paper.localeName;
		case LC_NAME: return name.localeName;
		case LC_ADDRESS: return address.localeName;
		case LC_TELEPHONE: return telephone.localeName;
		case LC_MEASUREMENT: return measurement.localeName;
		case LC_IDENTIFICATION: return identification.localeName;
		// special case: for LC_ALL, return the catch-all localeName
		case LC_ALL: return localeName;
	}

	__builtin_unreachable();
}

void localeinfo::resetCategory(int category) {
	__ensure(category != LC_ALL);

	switch (category) {
		case LC_CTYPE: ctype = {}; break;
		case LC_NUMERIC: numeric = {}; break;
		case LC_TIME: time = {}; break;
		case LC_COLLATE: collate = {}; break;
		case LC_MONETARY: monetary = {}; break;
		case LC_MESSAGES: messages = {}; break;
		case LC_PAPER: paper = {}; break;
		case LC_NAME: name = {}; break;
		case LC_ADDRESS: address = {}; break;
		case LC_TELEPHONE: telephone = {}; break;
		case LC_MEASUREMENT: measurement = {}; break;
		case LC_IDENTIFICATION: identification = {}; break;
	}
}

void localeinfo::updateLocaleName() {
	// if all categories have the same locale name, we do not need a composite name
	if (ctype.localeName == numeric.localeName && ctype.localeName == time.localeName
	    && ctype.localeName == collate.localeName && ctype.localeName == monetary.localeName
	    && ctype.localeName == messages.localeName && ctype.localeName == paper.localeName
	    && ctype.localeName == name.localeName && ctype.localeName == address.localeName
	    && ctype.localeName == telephone.localeName && ctype.localeName == measurement.localeName
	    && ctype.localeName == identification.localeName) {
		localeName = ctype.localeName;
		return;
	}

	localeName.detach();

	for (int category = LC_CTYPE; category <= LC_IDENTIFICATION; category++) {
		if (category == LC_ALL)
			continue;

		localeName += lcNames[category];
		localeName += '=';
		localeName += getCategoryLocaleName(category);
		if (category != LC_IDENTIFICATION)
			localeName += ';';
	}
}

// Apply the category for the locale `name` to the localeinfo object.
// `name` must be a valid, standalone locale name; composites are not supported!
bool applyCategory(int category, frg::string_view name, localeinfo *info) {
	if (name == "C" || name == "POSIX") {
		switch (category) {
			case LC_CTYPE:
				info->ctype.localeName = frg::string{name, getAllocator()};
				info->ctype = cLocale.ctype;
				break;
			case LC_NUMERIC:
				info->numeric.localeName = frg::string{name, getAllocator()};
				info->numeric = cLocale.numeric;
				break;
			case LC_TIME:
				info->time.localeName = frg::string{name, getAllocator()};
				info->time = cLocale.time;
				break;
			case LC_COLLATE:
				info->collate.localeName = frg::string{name, getAllocator()};
				info->collate = cLocale.collate;
				break;
			case LC_MONETARY:
				info->monetary.localeName = frg::string{name, getAllocator()};
				info->monetary = cLocale.monetary;
				break;
			case LC_MESSAGES:
				info->messages.localeName = frg::string{name, getAllocator()};
				info->messages = cLocale.messages;
				break;
			// skip LC_ALL
			case LC_PAPER:
				info->paper.localeName = frg::string{name, getAllocator()};
				info->paper = cLocale.paper;
				break;
			case LC_NAME:
				info->name.localeName = frg::string{name, getAllocator()};
				info->name = cLocale.name;
				break;
			case LC_ADDRESS:
				info->address.localeName = frg::string{name, getAllocator()};
				info->address = cLocale.address;
				break;
			case LC_TELEPHONE:
				info->telephone.localeName = frg::string{name, getAllocator()};
				info->telephone = cLocale.telephone;
				break;
			case LC_MEASUREMENT:
				info->measurement.localeName = frg::string{name, getAllocator()};
				info->measurement = cLocale.measurement;
				break;
			case LC_IDENTIFICATION:
				info->identification.localeName = frg::string{name, getAllocator()};
				info->identification = cLocale.identification;
				break;
			default:
				mlibc::infoLogger() << "mlibc: unhandled defaults for category "
					<< lcNames[category] << " in C/POSIX locale" << frg::endlog;
				return false;
		}

		return true;
	}

	openLocaleDatabase();

	if (findLocaleArchiveRecord(category, name, info)) {
		return true;
	} else if (findLocaleFileRecord(category, name, info)) {
		return true;
	}

	return false;
}

// Load a locale definition for the categories in mask.
// If `base` points to a nullptr, a new locale object is initialized, which defaults to C locale.
// Otherwise, the locale object in `base` is modified.
// `name` can be a locale name or a composite.
int loadLocale(int category_mask, frg::string_view name, localeinfo **base) {
	if((category_mask & LC_ALL_MASK) != category_mask)
		return EINVAL;

	if ((name == "POSIX" || name == "C") && category_mask == LC_ALL_MASK) {
		*base = frg::construct<localeinfo>(getAllocator());
		return 0;
	}

	auto composite = parseCompositeLocaleName(name);

	openLocaleDatabase();

	localeinfo *info = *base;

	if (!info)
		info = frg::construct<localeinfo>(getAllocator());

	bool error = false;

	while(category_mask) {
		auto mlibc_cat = __builtin_ctz(category_mask);
		__ensure(mlibc_cat != LC_ALL);

		frg::string_view cat_name = [&] () {
			if (composite) {
				return composite.value()[mlibc_cat];
			}
			return frg::string_view{name};
		}();

		if (cat_name == "POSIX" || cat_name == "C") {
			info->resetCategory(mlibc_cat);
		} else if (!findLocaleArchiveRecord(mlibc_cat, cat_name, info)) {
			error |= !findLocaleFileRecord(mlibc_cat, cat_name, info);
		}

		if (error)
			break;

		category_mask &= ~(1 << mlibc_cat);
	}

	if (error) {
		if (mlibc::globalConfig().debugLocale)
			mlibc::infoLogger() << "mlibc: Failed to load locale '" << (name.size() ? name : "(null)")
				<< "' for category mask 0x" << frg::hex_fmt{category_mask} << frg::endlog;

		if (!*base)
			frg::destruct(getAllocator(), info);
		return ENOENT;
	}

	info->updateLocaleName();
	*base = info;
	return 0;
}

#if __MLIBC_POSIX_OPTION
localeinfo *useThreadLocalLocale(localeinfo *loc) {
	localeinfo *old = current_locale ? current_locale : static_cast<localeinfo *>(LC_GLOBAL_LOCALE);
	if(loc == LC_GLOBAL_LOCALE)
		current_locale = nullptr;
	else if (loc)
		current_locale = reinterpret_cast<localeinfo *>(loc);

	return old;
}
#endif // __MLIBC_POSIX_OPTION

localeinfo *useGlobalLocale(localeinfo *loc) {
	localeinfo *old = current_global_locale;
	current_global_locale = reinterpret_cast<localeinfo *>(loc);
	return old;
}

void freeLocale(localeinfo *loc) {
	if(loc && loc != &startingLocale)
		frg::destruct(getAllocator(), reinterpret_cast<localeinfo *>(loc));
}

localeinfo *getActiveLocale() {
	if (current_locale)
		return current_locale;
	return current_global_locale;
}

localeinfo *getGlobalLocale() {
	return current_global_locale;
}

char *nl_langinfo(nl_item item) {
	return nl_langinfo_l(item, mlibc::getActiveLocale());
}

char *nl_langinfo_l(nl_item item, localeinfo * loc) {
	__ensure(loc != nullptr);
#ifdef LC_GLOBAL_LOCALE
	__ensure(loc != LC_GLOBAL_LOCALE);
#endif
	auto l = reinterpret_cast<localeinfo *>(loc);

	auto category = item >> 16;

	switch(item) {
		case CODESET:
			return const_cast<char *>(l->ctype.get(CODESET).asString().data());

		case ABDAY_1 ... ALT_DIGITS:
			return const_cast<char *>(l->time.get(item).asString().data());

		case DECIMAL_POINT:
			return const_cast<char *>(l->numeric.get(DECIMAL_POINT).asString().data());
		case THOUSEP:
			return const_cast<char *>(l->numeric.get(THOUSANDS_SEP).asString().data());
		case GROUPING:
			return const_cast<char *>(reinterpret_cast<const char *>(l->numeric.get(GROUPING).asByteSpan().data()));

		case YESEXPR ... NOSTR:
			return const_cast<char *>(l->messages.get(item).asString().data());

		case CRNCYSTR:
			return const_cast<char *>(l->monetary.get(CRNCYSTR).asString().data());
		case CURRENCY_SYMBOL:
			return const_cast<char *>(l->monetary.get(CURRENCY_SYMBOL).asString().data());

		default: {
			mlibc::infoLogger() << "mlibc: nl_langinfo item " << frg::hex_fmt{item & 0xFFFF}
				<< " of category " << lcNames[category] << " is missing" << frg::endlog;
			return const_cast<char *>("");
		}
	}
}

} // namespace mlibc
