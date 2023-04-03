#include <mqueue.h>
#include <bits/ensure.h>

int mq_getattr(mqd_t, struct mq_attr *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int mq_setattr(mqd_t, const struct mq_attr *__restrict__, struct mq_attr *__restrict__) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int mq_unlink(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

mqd_t mq_open(const char *, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
