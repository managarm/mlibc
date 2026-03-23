#include <abi-bits/fcntl.h>
#include <abi-bits/mode_t.h>
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mqueue.h>
#include <stdarg.h>

int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat) {
	if (int e = mlibc::sysdep_or_enosys<MqGetAttr>(mqdes, mqstat); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mq_setattr(mqd_t mqdes, const mq_attr *__restrict__ mqstat, mq_attr *__restrict__ omqstat) {
	if (int e = mlibc::sysdep_or_enosys<MqSetAttr>(mqdes, mqstat, omqstat); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mq_unlink(const char *name) {
	// Implementation-define behavior, but we follow linux in removing leading slashes.
	if (name[0] == '/')
		name++;

	if (int e = mlibc::sysdep_or_enosys<MqUnlink>(name); e) {
		errno = e;
		return -1;
	}
	return 0;
}

mqd_t mq_open(const char *name, int flags, ...) {
	mode_t mode = 0;
	mq_attr *attr = nullptr;

	// Implementation-define behavior, but we follow linux in removing leading slashes.
	if (name[0] == '/')
		name++;

	if (flags & O_CREAT) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		attr = va_arg(ap, mq_attr *);
		va_end(ap);
	}

	mqd_t out = 0;
	if (int e = mlibc::sysdep_or_enosys<MqOpen>(name, flags, mode, attr, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio) {
	if (int e = mlibc::sysdep_or_enosys<MqReceive>(mqdes, msg_ptr, msg_len, msg_prio); e) {
		errno = e;
		return -1;
	}
	return 0;
}
