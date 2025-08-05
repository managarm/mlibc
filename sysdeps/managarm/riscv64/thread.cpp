#include <bits/ensure.h>

extern "C" void __mlibc_start_thread() { __ensure(!"unimplemented"); }
