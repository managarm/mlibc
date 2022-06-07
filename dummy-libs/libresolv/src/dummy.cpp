
// We build an empty libresolv because some programs always links with -lresolv
// The actual functions reside inside libc

extern "C" void __mlibc_libresolv_dummy(void) { }

