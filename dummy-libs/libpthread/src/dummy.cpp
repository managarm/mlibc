
// We build an empty libpthread because g++ always links with -lpthread
// The actual functions reside inside libc

extern "C" void __mlibc_libpthread_dummy(void) { }

