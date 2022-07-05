
#ifndef _LANGINFO_H
#define _LANGINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/posix/locale_t.h>
#include <bits/nl_item.h>

char *nl_langinfo(nl_item);
char *nl_langinfo_1(nl_item, locale_t);

#ifdef __cplusplus
}
#endif

#endif // _LANGINFO_H

