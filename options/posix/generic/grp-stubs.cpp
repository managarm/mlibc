
#include <grp.h>
#include <bits/ensure.h>

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
struct group *getgrnam(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void setgrent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

