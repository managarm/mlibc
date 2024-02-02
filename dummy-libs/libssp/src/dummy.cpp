
// We build an empty libsso because some packages expect -lssp
// The actual ssp functions are provided by libc.

extern "C" void __mlibc_libssp_dummy(void) { }

