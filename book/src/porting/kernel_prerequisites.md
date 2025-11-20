# Kernel prerequisites

Before you attempt to port [`mlibc`](https://github.com/managarm/mlibc), ensure your kernel supports these things:
- Writing text to the screen or a serial port
- Basic paging and virtual memory operations
- Loading an ELF program, mapping a stack and jumping to the entry point
- Handling syscalls from userspace

For this port, we'll use [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), a small RISC-V kernel which supports the minimum functionality required for a mlibc 'hello world' program. Refer to this if you get stuck!

Note that a filesystem and storage drivers are not strictly necessary. In [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), we `include_bytes!` the contents of the user-space program into the kernel's executable. To support multiple files, you could include a trivial read-only filesystem like `tar` instead.
