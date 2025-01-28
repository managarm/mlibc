#include <sys/syscall.h>
#include <sys/sched2.h>
#include <errno.h>
#include <string.h>

extern "C" {

int get_thread_sched(void) {
	return 0;
}

int set_thread_sched(int flags) {
	return 0;
}

int set_deadlines(int runtime, int period) {
	return 0;
}

}
