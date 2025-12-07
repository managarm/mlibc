# Adding a new OS port

As mlibc has been designed with portability in mind, adding a new port is a straightfoward process:

1. Make sure your kernel has the [prerequisites](kernel_prerequisites.md) needed.
2. [Begin implementing the sysdeps](implementing_sysdeps_p1.md) to get access to mlibc headers.
3. [Build the toolchain](toolchain.md) for your OS.
4. [Finish implementing the sysdeps](implementing_sysdeps_p2).
5. Take a look at [what to do next](next_steps.md).

Make sure you have a directory for your operating system's sysroot and to store your toolchain. These will be referenced in code when needed as `$SYSROOT_DIR` and `$TOOLCHAIN_DIR` respectively.

The sysroot dir is where libraries and includes compiled for your OS will go. It is bascially a copy of your OS's root filesystem, and your compiler will look into it for libraries and header files when cross-compiling.

The toolchain dir will be where the cross-compilers will be installed.
