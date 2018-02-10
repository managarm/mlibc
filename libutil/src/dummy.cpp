
// We build an empty libutil because g++ always links with -lm
// The actual functions reside inside libc

extern "C" void __mlibc_libutil_dummy(void) { }

