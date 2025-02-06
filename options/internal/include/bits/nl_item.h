
#ifndef _NL_ITEM_H
#define _NL_ITEM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int nl_item;

#define ABDAY_1 0x60000
#define ABDAY_2 0x60001
#define ABDAY_3 0x60002
#define ABDAY_4 0x60003
#define ABDAY_5 0x60004
#define ABDAY_6 0x60005
#define ABDAY_7 0x60006

#define DAY_1 0x60007
#define DAY_2 0x60008
#define DAY_3 0x60009
#define DAY_4 0x6000A
#define DAY_5 0x6000B
#define DAY_6 0x6000C
#define DAY_7 0x6000D

#define ABMON_1 0x6000E
#define ABMON_2 0x6000F
#define ABMON_3 0x60010
#define ABMON_4 0x60011
#define ABMON_5 0x60012
#define ABMON_6 0x60013
#define ABMON_7 0x60014
#define ABMON_8 0x60015
#define ABMON_9 0x60016
#define ABMON_10 0x60017
#define ABMON_11 0x60018
#define ABMON_12 0x60019

#define MON_1 0x6001A
#define MON_2 0x6001B
#define MON_3 0x6001C
#define MON_4 0x6001D
#define MON_5 0x6001E
#define MON_6 0x6001F
#define MON_7 0x60020
#define MON_8 0x60021
#define MON_9 0x60022
#define MON_10 0x60023
#define MON_11 0x60024
#define MON_12 0x60025

#define AM_STR 0x60026
#define PM_STR 0x60027

#define D_T_FMT 0x60028
#define D_FMT 0x60029
#define T_FMT 0x6002A
#define T_FMT_AMPM 0x6002B

#define ERA 0x6002C
#define ERA_D_FMT 0x6002D
#define ALT_DIGITS 0x6002E
#define ERA_D_T_FMT 0x6002F
#define ERA_T_FMT 0x60030

#define CODESET 0x30000

#define CRNCYSTR 0x40000

#define RADIXCHAR 0x50000
#define DECIMAL_POINT RADIXCHAR
#define THOUSEP 0x50001
#define THOUSANDS_SEP THOUSEP

#define YESEXPR 0x70000
#define NOEXPR 0x70001

#ifdef __cplusplus
}
#endif

#endif /* _NL_ITEM_H */

