
#include <time.h>

namespace mlibc {

__attribute__ ((noreturn)) void sys_exit(int status);
int sys_clock_get(time_t *secs);

} //namespace mlibc

