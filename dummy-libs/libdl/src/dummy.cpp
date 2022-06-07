
// We build an empty libdl because g++ always links with -lm
// The actual functions reside inside libc

extern "C" void __mlibc_libdl_dummy(void) { }

