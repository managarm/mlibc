
// We build an empty libm because g++ always links with -lm
// The actual math functions reside inside libc

extern "C" void __mlibc_libm_dummy(void) { }

