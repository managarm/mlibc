#include <assert.h>
#include <termios.h>

static void check(speed_t speed) {
	struct termios tios = {};

	assert(!cfsetspeed(&tios, speed));
	assert(cfgetospeed(&tios) == speed);
	// POSIX lets an input speed of zero stand for the output speed, so a libc
	// that keeps the two apart is free to report either here.
	assert(cfgetispeed(&tios) == speed || cfgetispeed(&tios) == 0);
}

int main() {
	check(B0);
	check(B50);
	check(B9600);
	check(B115200);

	// Setting both speeds at once has to leave the same state behind as setting
	// them one at a time.
	struct termios both = {}, apart = {};
	assert(!cfsetspeed(&both, B38400));
	assert(!cfsetospeed(&apart, B38400));
	assert(!cfsetispeed(&apart, B38400));
	assert(cfgetospeed(&both) == cfgetospeed(&apart));

	return 0;
}
