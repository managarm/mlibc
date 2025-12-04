#include <mlibc/locale.hpp>

namespace mlibc {

nl_ctype::nl_ctype() {
	set(_NL_CTYPE_CLASS, frg::span<const uint8_t>{cLocaleCtypeClass});
	set(_NL_CTYPE_TOLOWER, frg::span<const uint32_t>{cLocaleCtypeToLower});
	set(_NL_CTYPE_TOUPPER, frg::span<const uint32_t>{cLocaleCtypeToUpper});
	set(_NL_CTYPE_CLASS32, frg::span<const uint8_t>{cLocaleCtypeClass32});
	set(_NL_CTYPE_CLASS_NAMES, {"upper\0" "lower\0" "alpha\0" "digit\0" "xdigit\0" "space\0" "print\0" "graph\0" "blank\0" "cntrl\0" "punct\0"  "alnum\0"});
	set(_NL_CTYPE_MAP_NAMES, {"toupper\0" "tolower\0"});
	set(_NL_CTYPE_MB_CUR_MAX, uint32_t{1});
	set(_NL_CTYPE_CODESET_NAME, {"ANSI_X3.4-1968"});
	set(_NL_CTYPE_CLASS_OFFSET, uint32_t{72});
	set(_NL_CTYPE_MAP_OFFSET, uint32_t{84});
	set(_NL_CTYPE_INDIGITS_MB_LEN, uint32_t{1});
	set(_NL_CTYPE_INDIGITS0_MB, {"0"});
	set(_NL_CTYPE_INDIGITS1_MB, {"1"});
	set(_NL_CTYPE_INDIGITS2_MB, {"2"});
	set(_NL_CTYPE_INDIGITS3_MB, {"3"});
	set(_NL_CTYPE_INDIGITS4_MB, {"4"});
	set(_NL_CTYPE_INDIGITS5_MB, {"5"});
	set(_NL_CTYPE_INDIGITS6_MB, {"6"});
	set(_NL_CTYPE_INDIGITS7_MB, {"7"});
	set(_NL_CTYPE_INDIGITS8_MB, {"8"});
	set(_NL_CTYPE_INDIGITS9_MB, {"9"});
	set(_NL_CTYPE_INDIGITS_WC_LEN, uint32_t{1});
	set(_NL_CTYPE_INDIGITS0_WC, {L"0"});
	set(_NL_CTYPE_INDIGITS1_WC, {L"1"});
	set(_NL_CTYPE_INDIGITS2_WC, {L"2"});
	set(_NL_CTYPE_INDIGITS3_WC, {L"3"});
	set(_NL_CTYPE_INDIGITS4_WC, {L"4"});
	set(_NL_CTYPE_INDIGITS5_WC, {L"5"});
	set(_NL_CTYPE_INDIGITS6_WC, {L"6"});
	set(_NL_CTYPE_INDIGITS7_WC, {L"7"});
	set(_NL_CTYPE_INDIGITS8_WC, {L"8"});
	set(_NL_CTYPE_INDIGITS9_WC, {L"9"});
	set(_NL_CTYPE_OUTDIGIT0_MB, {"0"});
	set(_NL_CTYPE_OUTDIGIT1_MB, {"1"});
	set(_NL_CTYPE_OUTDIGIT2_MB, {"2"});
	set(_NL_CTYPE_OUTDIGIT3_MB, {"3"});
	set(_NL_CTYPE_OUTDIGIT4_MB, {"4"});
	set(_NL_CTYPE_OUTDIGIT5_MB, {"5"});
	set(_NL_CTYPE_OUTDIGIT6_MB, {"6"});
	set(_NL_CTYPE_OUTDIGIT7_MB, {"7"});
	set(_NL_CTYPE_OUTDIGIT8_MB, {"8"});
	set(_NL_CTYPE_OUTDIGIT9_MB, {"9"});
	set(_NL_CTYPE_OUTDIGIT0_WC, {L'0'});
	set(_NL_CTYPE_OUTDIGIT1_WC, {L'1'});
	set(_NL_CTYPE_OUTDIGIT2_WC, {L'2'});
	set(_NL_CTYPE_OUTDIGIT3_WC, {L'3'});
	set(_NL_CTYPE_OUTDIGIT4_WC, {L'4'});
	set(_NL_CTYPE_OUTDIGIT5_WC, {L'5'});
	set(_NL_CTYPE_OUTDIGIT6_WC, {L'6'});
	set(_NL_CTYPE_OUTDIGIT7_WC, {L'7'});
	set(_NL_CTYPE_OUTDIGIT8_WC, {L'8'});
	set(_NL_CTYPE_OUTDIGIT9_WC, {L'9'});
	set(_NL_CTYPE_EXTRA_MAP_1, frg::span<const uint32_t>{cLocaleClassUpper});
	set(_NL_CTYPE_EXTRA_MAP_2, frg::span<const uint32_t>{cLocaleClassLower});
	set(_NL_CTYPE_EXTRA_MAP_3, frg::span<const uint32_t>{cLocaleClassAlpha});
	set(_NL_CTYPE_EXTRA_MAP_4, frg::span<const uint32_t>{cLocaleClassDigit});
	set(_NL_CTYPE_EXTRA_MAP_5, frg::span<const uint32_t>{cLocaleClassXdigit});
	set(_NL_CTYPE_EXTRA_MAP_6, frg::span<const uint32_t>{cLocaleClassSpace});
	set(_NL_CTYPE_EXTRA_MAP_7, frg::span<const uint32_t>{cLocaleClassPrint});
	set(_NL_CTYPE_EXTRA_MAP_8, frg::span<const uint32_t>{cLocaleClassGraph});
	set(_NL_CTYPE_EXTRA_MAP_9, frg::span<const uint32_t>{cLocaleClassBlank});
	set(_NL_CTYPE_EXTRA_MAP_10, frg::span<const uint32_t>{cLocaleClassCntrl});
	set(_NL_CTYPE_EXTRA_MAP_11, frg::span<const uint32_t>{cLocaleClassPunct});
	set(_NL_CTYPE_EXTRA_MAP_12, frg::span<const uint32_t>{cLocaleClassAlnum});
	set(_NL_CTYPE_EXTRA_MAP_13, frg::span<const uint32_t>{cLocaleMapToUpper});
	set(_NL_CTYPE_EXTRA_MAP_14, frg::span<const uint32_t>{cLocaleMapToLower});
}

nl_numeric::nl_numeric() {
	set(DECIMAL_POINT, {"."});
	set(THOUSEP, {""});
	set(GROUPING, frg::span<const uint8_t>{reinterpret_cast<const uint8_t *>(""), 1});
	set(_NL_NUMERIC_DECIMAL_POINT_WC, {L'.'});
	set(_NL_NUMERIC_THOUSANDS_SEP_WC, {L'\0'});
	set(_NL_NUMERIC_CODESET, {"ANSI_X3.4-1968"});
}

nl_time::nl_time() {
	set(ABDAY_1, {"Sun"});
	set(ABDAY_2, {"Mon"});
	set(ABDAY_3, {"Tue"});
	set(ABDAY_4, {"Wed"});
	set(ABDAY_5, {"Thu"});
	set(ABDAY_6, {"Fri"});
	set(ABDAY_7, {"Sat"});
	set(DAY_1, {"Sunday"});
	set(DAY_2, {"Monday"});
	set(DAY_3, {"Tuesday"});
	set(DAY_4, {"Wednesday"});
	set(DAY_5, {"Thursday"});
	set(DAY_6, {"Friday"});
	set(DAY_7, {"Saturday"});
	set(ABMON_1, {"Jan"});
	set(ABMON_2, {"Feb"});
	set(ABMON_3, {"Mar"});
	set(ABMON_4, {"Apr"});
	set(ABMON_5, {"May"});
	set(ABMON_6, {"Jun"});
	set(ABMON_7, {"Jul"});
	set(ABMON_8, {"Aug"});
	set(ABMON_9, {"Sep"});
	set(ABMON_10, {"Oct"});
	set(ABMON_11, {"Nov"});
	set(ABMON_12, {"Dec"});
	set(MON_1, {"January"});
	set(MON_2, {"February"});
	set(MON_3, {"March"});
	set(MON_4, {"April"});
	set(MON_5, {"May"});
	set(MON_6, {"June"});
	set(MON_7, {"July"});
	set(MON_8, {"August"});
	set(MON_9, {"September"});
	set(MON_10, {"October"});
	set(MON_11, {"November"});
	set(MON_12, {"December"});
	set(AM_STR, {"AM"});
	set(PM_STR, {"PM"});
	set(D_T_FMT, {"%a %b %e %H:%M:%S %Y"});
	set(D_FMT, {"%m/%d/%y"});
	set(T_FMT, {"%H:%M:%S"});
	set(T_FMT_AMPM, {"%I:%M:%S %p"});
	set(ERA, {""});
	set(ERA_YEAR, {""});
	set(ERA_D_FMT, {""});
	set(ALT_DIGITS, {""});
	set(ERA_D_T_FMT, {""});
	set(ERA_T_FMT, {""});
	set(_NL_TIME_ERA_NUM_ENTRIES, 0);
	set(_NL_TIME_ERA_ENTRIES, {""});
	set(_NL_WABDAY_1, {L"Sun"});
	set(_NL_WABDAY_2, {L"Mon"});
	set(_NL_WABDAY_3, {L"Tue"});
	set(_NL_WABDAY_4, {L"Wed"});
	set(_NL_WABDAY_5, {L"Thu"});
	set(_NL_WABDAY_6, {L"Fri"});
	set(_NL_WABDAY_7, {L"Sat"});
	set(_NL_WDAY_1, {L"Sunday"});
	set(_NL_WDAY_2, {L"Monday"});
	set(_NL_WDAY_3, {L"Tuesday"});
	set(_NL_WDAY_4, {L"Wednesday"});
	set(_NL_WDAY_5, {L"Thursday"});
	set(_NL_WDAY_6, {L"Friday"});
	set(_NL_WDAY_7, {L"Saturday"});
	set(_NL_WABMON_1, {L"Jan"});
	set(_NL_WABMON_2, {L"Feb"});
	set(_NL_WABMON_3, {L"Mar"});
	set(_NL_WABMON_4, {L"Apr"});
	set(_NL_WABMON_5, {L"May"});
	set(_NL_WABMON_6, {L"Jun"});
	set(_NL_WABMON_7, {L"Jul"});
	set(_NL_WABMON_8, {L"Aug"});
	set(_NL_WABMON_9, {L"Sep"});
	set(_NL_WABMON_10, {L"Oct"});
	set(_NL_WABMON_11, {L"Nov"});
	set(_NL_WABMON_12, {L"Dec"});
	set(_NL_WMON_1, {L"January"});
	set(_NL_WMON_2, {L"February"});
	set(_NL_WMON_3, {L"March"});
	set(_NL_WMON_4, {L"April"});
	set(_NL_WMON_5, {L"May"});
	set(_NL_WMON_6, {L"June"});
	set(_NL_WMON_7, {L"July"});
	set(_NL_WMON_8, {L"August"});
	set(_NL_WMON_9, {L"September"});
	set(_NL_WMON_10, {L"October"});
	set(_NL_WMON_11, {L"November"});
	set(_NL_WMON_12, {L"December"});
	set(_NL_WAM_STR, {L"AM"});
	set(_NL_WPM_STR, {L"PM"});
	set(_NL_WD_T_FMT, {L"%a %b %e %H:%M:%S %Y"});
	set(_NL_WD_FMT, {L"%m/%d/%y"});
	set(_NL_WT_FMT, {L"%H:%M:%S"});
	set(_NL_WT_FMT_AMPM, {L"%I:%M:%S %p"});
	set(_NL_WERA_YEAR, {L""});
	set(_NL_WERA_D_FMT, {L""});
	set(_NL_WALT_DIGITS, {L""});
	set(_NL_WERA_D_T_FMT, {L""});
	set(_NL_WERA_T_FMT, {L""});
	set(_NL_TIME_WEEK_NDAYS, {"\7"});
	set(_NL_TIME_WEEK_1STDAY, 19971130);
	set(_NL_TIME_WEEK_1STWEEK, {"\4"});
	set(_NL_TIME_FIRST_WEEKDAY, {"\1"});
	set(_NL_TIME_FIRST_WORKDAY, {"\2"});
	set(_NL_TIME_CAL_DIRECTION, {"\1"});
	set(_NL_TIME_TIMEZONE, {""});
	set(_DATE_FMT, {"%a %b %e %H:%M:%S %Z %Y"});
	set(_NL_W_DATE_FMT, {L"%a %b %e %H:%M:%S %Z %Y"});
	set(_NL_TIME_CODESET, {"ANSI_X3.4-1968"});
	set(ALTMON_1, {"January"});
	set(ALTMON_2, {"February"});
	set(ALTMON_3, {"March"});
	set(ALTMON_4, {"April"});
	set(ALTMON_5, {"May"});
	set(ALTMON_6, {"June"});
	set(ALTMON_7, {"July"});
	set(ALTMON_8, {"August"});
	set(ALTMON_9, {"September"});
	set(ALTMON_10, {"October"});
	set(ALTMON_11, {"November"});
	set(ALTMON_12, {"December"});
	set(_NL_WALTMON_1, {L"January"});
	set(_NL_WALTMON_2, {L"February"});
	set(_NL_WALTMON_3, {L"March"});
	set(_NL_WALTMON_4, {L"April"});
	set(_NL_WALTMON_5, {L"May"});
	set(_NL_WALTMON_6, {L"June"});
	set(_NL_WALTMON_7, {L"July"});
	set(_NL_WALTMON_8, {L"August"});
	set(_NL_WALTMON_9, {L"September"});
	set(_NL_WALTMON_10, {L"October"});
	set(_NL_WALTMON_11, {L"November"});
	set(_NL_WALTMON_12, {L"December"});
	set(_NL_ABALTMON_1, {"Jan"});
	set(_NL_ABALTMON_2, {"Feb"});
	set(_NL_ABALTMON_3, {"Mar"});
	set(_NL_ABALTMON_4, {"Apr"});
	set(_NL_ABALTMON_5, {"May"});
	set(_NL_ABALTMON_6, {"Jun"});
	set(_NL_ABALTMON_7, {"Jul"});
	set(_NL_ABALTMON_8, {"Aug"});
	set(_NL_ABALTMON_9, {"Sep"});
	set(_NL_ABALTMON_10, {"Oct"});
	set(_NL_ABALTMON_11, {"Nov"});
	set(_NL_ABALTMON_12, {"Dec"});
	set(_NL_WABALTMON_1, {L"Jan"});
	set(_NL_WABALTMON_2, {L"Feb"});
	set(_NL_WABALTMON_3, {L"Mar"});
	set(_NL_WABALTMON_4, {L"Apr"});
	set(_NL_WABALTMON_5, {L"May"});
	set(_NL_WABALTMON_6, {L"Jun"});
	set(_NL_WABALTMON_7, {L"Jul"});
	set(_NL_WABALTMON_8, {L"Aug"});
	set(_NL_WABALTMON_9, {L"Sep"});
	set(_NL_WABALTMON_10, {L"Oct"});
	set(_NL_WABALTMON_11, {L"Nov"});
	set(_NL_WABALTMON_12, {L"Dec"});
}

extern frg::array<const char, 256> cLocaleCollSeqMb;
extern frg::array<const uint32_t, 263> cLocaleCollSeqWc;

nl_collate::nl_collate() {
	set(_NL_COLLATE_NRULES, 0);
	set(_NL_COLLATE_RULESETS, frg::span<const uint8_t>{});
	set(_NL_COLLATE_TABLEMB, frg::span<const uint8_t>{});
	set(_NL_COLLATE_WEIGHTMB, frg::span<const uint8_t>{});
	set(_NL_COLLATE_EXTRAMB, frg::span<const uint8_t>{});
	set(_NL_COLLATE_INDIRECTMB, frg::span<const uint8_t>{});
	set(_NL_COLLATE_GAP1, frg::span<const uint8_t>{});
	set(_NL_COLLATE_GAP2, frg::span<const uint8_t>{});
	set(_NL_COLLATE_GAP3, frg::span<const uint8_t>{});
	set(_NL_COLLATE_TABLEWC, frg::span<const uint8_t>{});
	set(_NL_COLLATE_WEIGHTWC, frg::span<const uint8_t>{});
	set(_NL_COLLATE_EXTRAWC, frg::span<const uint8_t>{});
	set(_NL_COLLATE_INDIRECTWC, frg::span<const uint8_t>{});
	set(_NL_COLLATE_SYMB_HASH_SIZEMB, 0);
	set(_NL_COLLATE_SYMB_TABLEMB, frg::span<const uint8_t>{});
	set(_NL_COLLATE_SYMB_EXTRAMB, frg::span<const uint8_t>{});
	set(_NL_COLLATE_COLLSEQMB, frg::span<const uint8_t>{reinterpret_cast<const uint8_t *>(cLocaleCollSeqMb.data()), cLocaleCollSeqMb.size()});
	set(_NL_COLLATE_COLLSEQWC, frg::span<const uint32_t>{cLocaleCollSeqWc});
	set(_NL_COLLATE_CODESET, {"ANSI_X3.4-1968"});
}

nl_monetary::nl_monetary() {
	set(INT_CURR_SYMBOL, {""});
	set(CURRENCY_SYMBOL, {""});
	set(MON_DECIMAL_POINT, {""});
	set(MON_THOUSANDS_SEP, {""});
	set(MON_GROUPING, frg::span<const uint8_t>{reinterpret_cast<const uint8_t *>(""), 1});
	set(POSITIVE_SIGN, {""});
	set(NEGATIVE_SIGN, {""});
	set(INT_FRAC_DIGITS, '\xff');
	set(FRAC_DIGITS, '\xff');
	set(P_CS_PRECEDES, '\xff');
	set(P_SEP_BY_SPACE, '\xff');
	set(N_CS_PRECEDES, '\xff');
	set(N_SEP_BY_SPACE, '\xff');
	set(P_SIGN_POSN, '\xff');
	set(N_SIGN_POSN, '\xff');
	set(CRNCYSTR, {"-"});
	set(INT_P_CS_PRECEDES, '\xff');
	set(INT_P_SEP_BY_SPACE, '\xff');
	set(INT_N_CS_PRECEDES, '\xff');
	set(INT_N_SEP_BY_SPACE, '\xff');
	set(INT_P_SIGN_POSN, '\xff');
	set(INT_N_SIGN_POSN, '\xff');
	set(_NL_MONETARY_CODESET, {"ANSI_X3.4-1968"});
}

nl_messages::nl_messages() {
	set(YESEXPR, {"^[yY]"});
	set(NOEXPR, {"^[nN]"});
	set(YESSTR, {""});
	set(NOSTR, {""});
	set(_NL_MESSAGES_CODESET, {"ANSI_X3.4-1968"});
}

nl_paper::nl_paper() {
	set(_NL_PAPER_HEIGHT, 297);
	set(_NL_PAPER_WIDTH, 210);
	set(_NL_PAPER_CODESET, {"ANSI_X3.4-1968"});
}

nl_name::nl_name() {
	set(_NL_NAME_NAME_FMT, {"%p%t%g%t%m%t%f"});
	set(_NL_NAME_NAME_GEN, {""});
	set(_NL_NAME_NAME_MR, {""});
	set(_NL_NAME_NAME_MRS, {""});
	set(_NL_NAME_NAME_MISS, {""});
	set(_NL_NAME_NAME_MS, {""});
	set(_NL_NAME_CODESET, {"ANSI_X3.4-1968"});
}

nl_address::nl_address() {
	set(_NL_ADDRESS_POSTAL_FMT, {"%a%N%f%N%d%N%b%N%s %h %e %r%N%C-%z %T%N%c%N"});
	set(_NL_ADDRESS_COUNTRY_NAME, {""});
	set(_NL_ADDRESS_COUNTRY_POST, {""});
	set(_NL_ADDRESS_COUNTRY_AB2, {""});
	set(_NL_ADDRESS_COUNTRY_AB3, {""});
	set(_NL_ADDRESS_COUNTRY_CAR, {""});
	set(_NL_ADDRESS_COUNTRY_NUM, 0);
	set(_NL_ADDRESS_COUNTRY_ISBN, {""});
	set(_NL_ADDRESS_LANG_NAME, {""});
	set(_NL_ADDRESS_LANG_AB, {""});
	set(_NL_ADDRESS_LANG_TERM, {""});
	set(_NL_ADDRESS_LANG_LIB, {""});
	set(_NL_ADDRESS_CODESET, {"ANSI_X3.4-1968"});
}

nl_telephone::nl_telephone() {
	set(_NL_TELEPHONE_TEL_INT_FMT, {"+%c %a %l"});
	set(_NL_TELEPHONE_TEL_DOM_FMT, {""});
	set(_NL_TELEPHONE_INT_SELECT, {""});
	set(_NL_TELEPHONE_INT_PREFIX, {""});
	set(_NL_TELEPHONE_CODESET, {"ANSI_X3.4-1968"});
}

nl_measurement::nl_measurement() {
	set(_NL_MEASUREMENT_MEASUREMENT, {"\1"});
	set(_NL_MEASUREMENT_CODESET, {"ANSI_X3.4-1968"});
}

nl_identification::nl_identification() {
	set(_NL_IDENTIFICATION_TITLE, {"ISO/IEC 14652 i18n FDCC-set"});
	set(_NL_IDENTIFICATION_SOURCE, {"ISO/IEC Copyright Office"});
	set(_NL_IDENTIFICATION_ADDRESS, {"Case postale 56, CH-1211 Geneve 20, Switzerland"});
	set(_NL_IDENTIFICATION_CONTACT, {""});
	set(_NL_IDENTIFICATION_EMAIL, {""});
	set(_NL_IDENTIFICATION_TEL, {""});
	set(_NL_IDENTIFICATION_FAX, {""});
	set(_NL_IDENTIFICATION_LANGUAGE, {""});
	set(_NL_IDENTIFICATION_TERRITORY, {"ISO"});
	set(_NL_IDENTIFICATION_AUDIENCE, {""});
	set(_NL_IDENTIFICATION_APPLICATION, {""});
	set(_NL_IDENTIFICATION_ABBREVIATION, {""});
	set(_NL_IDENTIFICATION_REVISION, {"1.0"});
	set(_NL_IDENTIFICATION_DATE, {"2001-12-08"});
	set(_NL_IDENTIFICATION_CATEGORY, {
		"i18n:1999\0" "i18n:1999\0" "i18n:1999\0" "i18n:1999\0"
		"i18n:1999\0" "i18n:1999\0" "\0" "i18n:1999\0"
		"i18n:1999\0" "i18n:1999\0" "i18n:1999\0" "i18n:1999\0"
		"i18n:1999\0" "i18n:1999\0" "i18n:1999\0" "i18n:1999\0"
		"i18n:1999"
	});
	set(_NL_IDENTIFICATION_CODESET, {"ANSI_X3.4-1968"});
}

} // namespace mlibc
