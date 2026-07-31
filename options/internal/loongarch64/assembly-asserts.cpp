#include <stddef.h>
#include <setjmp.h>
#include <mlibc-asm/jmp_buf.h>

static_assert(offsetof(__jmp_buf, __savesigs) == JMP_BUF_SAVESIGS_OFFSET);
