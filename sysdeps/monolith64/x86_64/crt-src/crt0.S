.section .text
.global _start
.type _start, @function

_start:
    # Call global constructors (if any)
    call __mlibc_entry

    # If __libc_entry returns, exit with its return code
    mov %rax, %rdi
    call _exit

    hlt  # just in case

