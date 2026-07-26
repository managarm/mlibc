#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <sys/sysinfo.h>
#include <unistd.h>

int get_nprocs(void) { return sysconf(_SC_NPROCESSORS_ONLN); }

int get_nprocs_conf(void) { return sysconf(_SC_NPROCESSORS_CONF); }
