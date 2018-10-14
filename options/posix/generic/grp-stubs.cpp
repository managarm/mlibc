
#include <grp.h>
#include <string.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>

namespace {

char **global_empty_strlist = { nullptr };

thread_local struct group local_group;

} // anonymous namespace

void endgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct group *getgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct group *getgrgid(gid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct group *getgrnam(const char *name) {
	mlibc::infoLogger() << "mlibc: getgrname() for group '"
			<< name << "' returns null data" << frg::endlog;
	local_group.gr_name = strdup(name);
	local_group.gr_gid = 0;
	local_group.gr_mem = global_empty_strlist;
	return &local_group;
}
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void setgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setgroups(size_t size, const gid_t *list) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

