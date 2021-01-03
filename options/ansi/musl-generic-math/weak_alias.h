#ifndef _WEAK_ALIAS_H
#define _WEAK_ALIAS_H

#define weak_alias(name, alias) \
	extern __typeof (name) alias __attribute__ ((weak, alias (#name)));

#endif
