#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>

namespace mlibc {

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

}
