
// We build an empty libutil because g++ always links with -lutil
// The actual functions reside inside libc

extern "C" void __mlibc_libutil_dummy(void) { }

