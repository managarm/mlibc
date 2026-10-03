
// We build this lib because some packages expect -lssp_nonshared
// The actual ssp functions are provided by libc.

// On 32-bit x86 (and powerpc), GCC emits hidden references to
// __stack_chk_fail_local instead of __stack_chk_fail to avoid paying for a
// PLT entry.
extern "C" [[noreturn]] void __stack_chk_fail();

extern "C" [[noreturn, gnu::visibility("hidden")]] void __stack_chk_fail_local() {
	__stack_chk_fail();
}
