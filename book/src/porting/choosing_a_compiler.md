# Choosing a compiler

To compile [`mlibc`](https://github.com/managarm/mlibc) and any userspace programs which link against it, you'll need a suitable compiler. For that, you will need to build a full [OS Specific Toolchain](https://wiki.osdev.org/OS_Specific_Toolchain). Using your host's compiler or a generic toolchain could lead into strange build failures and is not recommended nor supported.

After that, you'll have a compiler that implicitly links against (m)libc. For [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), we'll rely on the host's `riscv64-linux-gnu-gcc` compiler to avoid setting up a hosted OS-specific compiler for a brief demo. The `CC` variable will be used to indicate the C compiler in Makefiles.

## Creating a Meson cross file

[`mlibc`](https://github.com/managarm/mlibc) uses a build system called [Meson](https://mesonbuild.com/). When cross-compiling, you must tell meson about your compiler and target via a [_cross file_](https://mesonbuild.com/Cross-compilation.html).

For [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), our cross file looks like this:

```toml
{{#include ../../../ci/demo.cross-file}}
```
