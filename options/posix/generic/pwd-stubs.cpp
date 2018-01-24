
#include <pwd.h>

#include <bits/ensure.h>

// endpwd() is provided by the platform
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
// getpwuid() is provided by the platform
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void setpwent(void) {
	__ensure(!"Not implemented");
}

