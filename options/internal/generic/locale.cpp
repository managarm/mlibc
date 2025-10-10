#include <frg/allocation.hpp>
#include <locale.h>
#include <mlibc/file-window.hpp>
#include <mlibc/locale-archive-parsing.hpp>
#include <mlibc/locale.hpp>

namespace {

struct localeinfo {
	mlibc::nl_numeric numeric = {};
	mlibc::nl_messages messages = {};
	mlibc::nl_monetary monetary = {};
	mlibc::nl_time time = {};
};

thread_local localeinfo *current_locale = nullptr;

frg::optional<file_window> localeDatabaseFd;

// open the locale-archive database, if it exists
// TODO: what should we do about systems with no archive?
void openLocaleDatabase() {
#if !MLIBC_MAP_FILE_WINDOWS
	return;
#endif

	if (localeDatabaseFd)
		return;

	struct stat info;
	if (mlibc::sys_stat(mlibc::fsfd_target::path, -1, "/usr/lib/locale/locale-archive", 0, &info))
		return;

	file_window window{"/usr/lib/locale/locale-archive"};

	LocaleArchive::Header *header = static_cast<LocaleArchive::Header *>(window.get());
	if (header->magic != LocaleArchive::HEADER_MAGIC) {
		if (debugLocale)
			mlibc::infoLogger() << "mlibc: Locale database has invalid magic" << frg::endlog;
		return;
	}

	localeDatabaseFd = std::move(window);
}

frg::optional<off_t> findLocaleRecord(const char *name) {
	__ensure(localeDatabaseFd);

	auto header = static_cast<LocaleArchive::Header *>(localeDatabaseFd->get());
	if (header->namehash_size <= 2)
		return frg::null_opt;
	auto namehashtab = reinterpret_cast<LocaleArchive::NameHashEntry *>((uint8_t *) localeDatabaseFd->get() + header->namehash_offset);

	auto hash = nameHashVal<uint32_t>(name, strlen(name));
	auto idx = hash % header->namehash_size;
	auto incr = 1 + hash % (header->namehash_size - 2);

	while(true) {
		if (namehashtab[idx].name_offset == 0)
			return frg::null_opt;

		auto current_entry_name = (char *) localeDatabaseFd->get() + namehashtab[idx].name_offset;

		if (namehashtab[idx].hashval == hash && !strcmp(name, current_entry_name))
			return namehashtab[idx].locrec_offset;

		idx += incr;
		if (idx >= header->namehash_size)
			idx -= header->namehash_size;
	}

	return frg::null_opt;
}

bool parseLocaleInfo(int category, const char *name, off_t recordOff, localeinfo *out) {
	__ensure(localeDatabaseFd);
	__ensure(category != GLIBC_LC_ALL);

	auto locrec = reinterpret_cast<LocaleArchive::LocaleRecord *>((uint8_t *) localeDatabaseFd->get() + recordOff);
	if (locrec->record[category].offset == 0 || locrec->record[category].len == 0) {
		if (debugLocale)
			mlibc::infoLogger() << "mlibc: Locale '" << name
				<< "' does not support category " << category << frg::endlog;
		return false;
	}

	uint8_t *record = reinterpret_cast<uint8_t *>(localeDatabaseFd->get()) + locrec->record[category].offset;
	frg::span<uint8_t> rec{record, locrec->record[category].len};

	uint32_t magic;
	memcpy(&magic, record, sizeof(magic));
	if (magic != categoryMagic(category)) {
		if (debugLocale)
			mlibc::infoLogger() << "mlibc: Locale record for '" << name
				<< "' in category " << category
				<< " has invalid magic" << frg::endlog;
		return false;
	}

	uint32_t elements;
	memcpy(&elements, record + 4, sizeof(elements));
	frg::span<const uint32_t> offsets{(uint32_t *)(record + 8), elements};

	switch(category) {
		case GLIBC_LC_NUMERIC: {
			parse_category(std::tie(
				out->numeric.decimal_point,
				out->numeric.thousands_sep,
				out->numeric.grouping,
				out->numeric.numeric_decimal_point_wc,
				out->numeric.numeric_thousands_sep_wc,
				out->numeric.codeset
			), numeric_parser, rec, offsets);
			break;
		}
		case GLIBC_LC_MESSAGES: {
			parse_category(std::tie(
				out->messages.yesexpr,
				out->messages.noexpr,
				out->messages.yesstr,
				out->messages.nostr,
				out->messages.codeset
			), messages_parser, rec, offsets);
			break;
		}
		case GLIBC_LC_MONETARY: {
			parse_category(std::tie(
				out->monetary.int_curr_symbol,
				out->monetary.currency_symbol,
				out->monetary.mon_decimal_point,
				out->monetary.mon_thousands_sep,
				out->monetary.mon_grouping,
				out->monetary.positive_sign,
				out->monetary.negative_sign,
				out->monetary.int_frac_digits,
				out->monetary.frac_digits,
				out->monetary.p_cs_precedes,
				out->monetary.p_sep_by_space,
				out->monetary.n_cs_precedes,
				out->monetary.n_sep_by_space,
				out->monetary.p_sign_posn,
				out->monetary.n_sign_posn,
				out->monetary.crncystr
			), monetary_parser, rec, offsets);
			break;
		}
		case GLIBC_LC_TIME: {
			parse_category(std::tie(
				out->time.abday,
				out->time.day,
				out->time.abmon,
				out->time.mon,
				out->time.am_pm,
				out->time.d_t_fmt,
				out->time.d_fmt,
				out->time.t_fmt,
				out->time.t_fmt_ampm,
				out->time.era,
				out->time.era_year,
				out->time.era_d_fmt,
				out->time.alt_digits,
				out->time.era_d_t_fmt,
				out->time.era_t_fmt
			), time_parser, rec, offsets);
			break;
		}
		default:
			mlibc::infoLogger() << "mlibc: Category 0x"
				<< frg::hex_fmt{category} << " not yet supported" << frg::endlog;
			return false;
	}

	return true;
}

} // namespace

namespace mlibc {

locale_t loadLocale(int category_mask, const char *name, locale_t base) {
	if(!category_mask)
		return nullptr;

	if (!strcmp(name, "POSIX") || !strcmp(name, "C"))
		return frg::construct<localeinfo>(getAllocator());

	openLocaleDatabase();

	if (!localeDatabaseFd) {
		if (debugLocale)
			mlibc::infoLogger() << "mlibc: No locale database found, cannot load locale "
				<< (name ? name : "(null)") << frg::endlog;
		return (locale_t) nullptr;
	}

	localeinfo *info;

	if (base) {
		info = reinterpret_cast<localeinfo *>(base);
	} else {
		info = frg::construct<localeinfo>(getAllocator());
	}

	auto recordRes = findLocaleRecord(name);
	if (!recordRes) {
		if (debugLocale)
			mlibc::infoLogger() << "mlibc: Locale '" << name
				<< "' not found in locale database" << frg::endlog;
		return nullptr;
	}

	bool error = false;

	if (category_mask & LC_NUMERIC_MASK)
		error |= !parseLocaleInfo(GLIBC_LC_NUMERIC, name, *recordRes, info);
	if (category_mask & LC_MESSAGES_MASK)
		error |= !parseLocaleInfo(GLIBC_LC_MESSAGES, name, *recordRes, info);
	if (category_mask & LC_MONETARY_MASK)
		error |= !parseLocaleInfo(GLIBC_LC_MONETARY, name, *recordRes, info);
	if (category_mask & LC_TIME_MASK)
		error |= !parseLocaleInfo(GLIBC_LC_TIME, name, *recordRes, info);

	if (error) {
		if (debugLocale)
			mlibc::infoLogger() << "mlibc: Failed to load locale " << (name ? name : "(null)")
				<< " for category mask 0x" << frg::hex_fmt{category_mask} << frg::endlog;

		if (!base)
			frg::destruct(getAllocator(), info);
		return (locale_t) nullptr;
	}

	return info;
}

locale_t useThreadLocalLocale(locale_t loc) {
	localeinfo *old = current_locale;
	current_locale = reinterpret_cast<localeinfo *>(loc);
	return old;
}

void freeLocale(locale_t loc) {
	if(loc)
		frg::destruct(getAllocator(), reinterpret_cast<localeinfo *>(loc));
}

char *nl_langinfo(nl_item item) {
	if(item == CODESET) {
		return const_cast<char *>("UTF-8");
	} else if(item >= ABMON_1 && item <= ABMON_12) {
		switch(item) {
			case ABMON_1: return const_cast<char *>("Jan");
			case ABMON_2: return const_cast<char *>("Feb");
			case ABMON_3: return const_cast<char *>("Mar");
			case ABMON_4: return const_cast<char *>("Apr");
			case ABMON_5: return const_cast<char *>("May");
			case ABMON_6: return const_cast<char *>("Jun");
			case ABMON_7: return const_cast<char *>("Jul");
			case ABMON_8: return const_cast<char *>("Aug");
			case ABMON_9: return const_cast<char *>("Sep");
			case ABMON_10: return const_cast<char *>("Oct");
			case ABMON_11: return const_cast<char *>("Nov");
			case ABMON_12: return const_cast<char *>("Dec");
			default:
				__ensure(!"ABMON_* constants don't seem to be contiguous!");
				__builtin_unreachable();
		}
	} else if(item >= MON_1 && item <= MON_12) {
		switch(item) {
			case MON_1: return const_cast<char *>("January");
			case MON_2: return const_cast<char *>("Feburary");
			case MON_3: return const_cast<char *>("March");
			case MON_4: return const_cast<char *>("April");
			case MON_5: return const_cast<char *>("May");
			case MON_6: return const_cast<char *>("June");
			case MON_7: return const_cast<char *>("July");
			case MON_8: return const_cast<char *>("August");
			case MON_9: return const_cast<char *>("September");
			case MON_10: return const_cast<char *>("October");
			case MON_11: return const_cast<char *>("November");
			case MON_12: return const_cast<char *>("December");
			default:
				__ensure(!"MON_* constants don't seem to be contiguous!");
				__builtin_unreachable();
		}
	} else if(item == AM_STR) {
		return const_cast<char *>("AM");
	} else if(item == PM_STR) {
		return const_cast<char *>("PM");
	} else if(item >= DAY_1 && item <= DAY_7) {
		switch(item) {
			case DAY_1: return const_cast<char *>("Sunday");
			case DAY_2: return const_cast<char *>("Monday");
			case DAY_3: return const_cast<char *>("Tuesday");
			case DAY_4: return const_cast<char *>("Wednesday");
			case DAY_5: return const_cast<char *>("Thursday");
			case DAY_6: return const_cast<char *>("Friday");
			case DAY_7: return const_cast<char *>("Saturday");
			default:
				__ensure(!"DAY_* constants don't seem to be contiguous!");
				__builtin_unreachable();
		}
	} else if(item >= ABDAY_1 && item <= ABDAY_7) {
		switch(item) {
			case ABDAY_1: return const_cast<char *>("Sun");
			case ABDAY_2: return const_cast<char *>("Mon");
			case ABDAY_3: return const_cast<char *>("Tue");
			case ABDAY_4: return const_cast<char *>("Wed");
			case ABDAY_5: return const_cast<char *>("Thu");
			case ABDAY_6: return const_cast<char *>("Fri");
			case ABDAY_7: return const_cast<char *>("Sat");
			default:
				__ensure(!"ABDAY_* constants don't seem to be contiguous!");
				__builtin_unreachable();
		}
	}else if(item == D_FMT) {
		return const_cast<char *>("%m/%d/%y");
	}else if(item == T_FMT) {
		return const_cast<char *>("%H:%M:%S");
	}else if(item == T_FMT_AMPM) {
		return const_cast<char *>("%I:%M:%S %p");
	}else if(item == D_T_FMT) {
		return const_cast<char *>("%a %b %e %T %Y");
	} else if (item == RADIXCHAR) {
		return const_cast<char *>(".");
	} else if (item == THOUSEP) {
		return const_cast<char *>("");
	}else if(item == YESEXPR) {
		return const_cast<char *>("^[yY]");
	}else if(item == NOEXPR) {
		return const_cast<char *>("^[nN]");
	}else{
		mlibc::infoLogger() << "mlibc: nl_langinfo item "
				<< item << " is not implemented properly" << frg::endlog;
		return const_cast<char *>("");
	}
}

char *nl_langinfo_l(nl_item item, locale_t loc) {
	__ensure(loc != (locale_t) nullptr);
	__ensure(loc != LC_GLOBAL_LOCALE);
	auto l = reinterpret_cast<localeinfo *>(loc);

	switch(item) {
		case ABDAY_1: return strndup(l->time.abday[0].data(), l->time.abday[0].size());
		case DAY_1: return strndup(l->time.day[0].data(), l->time.day[0].size());
		case THOUSEP: return l->numeric.thousands_sep.data();
		case DECIMAL_POINT: return l->numeric.decimal_point.data();
		case YESEXPR: return l->messages.yesexpr.data();
		case NOEXPR: return l->messages.noexpr.data();
		case CURRENCY_SYMBOL: return l->monetary.currency_symbol.data();
		case CRNCYSTR: return l->monetary.crncystr.data();
		default: {
			mlibc::infoLogger() << "mlibc: nl_langinfo_l item " << frg::hex_fmt{item}
				<< " is not implemented properly" << frg::endlog;
			return const_cast<char *>("");
		}
	}
}

} // namespace mlibc
