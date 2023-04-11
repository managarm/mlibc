#ifndef _WEAK_ALIAS_H
#define _WEAK_ALIAS_H

#define weak_alias(name, alias_to) \
	extern __typeof (name) alias_to __attribute__((__weak__, __alias__(#name)));

#endif
