#include <gshadow.h>
#include <bits/ensure.h>

int getsgnam_r(const char *, struct sgrp *, char *, size_t, struct sgrp **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
