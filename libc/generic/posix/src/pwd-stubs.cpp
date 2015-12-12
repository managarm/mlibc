
#include <pwd.h>

#include <mlibc/ensure.h>

void endpwent(void) {
	__ensure(!"Not implemented");
}
struct passwd *getpwent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct passwd *getpwnam(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
struct passwd *getpwuid(uid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void setpwent(void) {
	__ensure(!"Not implemented");
}

