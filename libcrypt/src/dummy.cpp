
// We build an empty libcrypt because shadow expects -lcrypt
// The actual crypt functions reside inside libc

extern "C" void __mlibc_libcrypt_dummy(void) { }

