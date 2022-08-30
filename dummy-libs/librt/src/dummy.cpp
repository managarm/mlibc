
// We build an empty librt because g++ always links with -lrt
// The actual functions reside inside libc

extern "C" void __mlibc_librt_dummy(void) { }

