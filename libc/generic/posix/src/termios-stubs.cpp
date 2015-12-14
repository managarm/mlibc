
#include <termios.h>

#include <mlibc/ensure.h>

speed_t cfgetispeed(const struct termios *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
speed_t cfgetospeed(const struct termios *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int cfsetispeed(struct termios *, speed_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int cfsetospeed(struct termios *, speed_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcdrain(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcflow(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcflush(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// tcgetattr() is provided by the platform
pid_t tcgetsid(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcsendbreak(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcsetattr(int, int, const struct termios *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

