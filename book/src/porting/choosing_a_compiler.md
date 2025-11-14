# Choosing a compiler

To compile [`mlibc`](https://github.com/managarm/mlibc) and any userspace programs which link against it, you'll need a suitable compiler. Roughly speaking you have two choices:

1. **Build a full [OS Specific Toolchain](https://wiki.osdev.org/OS_Specific_Toolchain).**
    
    With this, you'll have a compiler that implicitly links against (m)libc, so we recommend doing this _after_ you have a basic port working.

1. **Use a generic ELF toolchain.**

    If compiling your kernel with a generic toolchain like `x86_64-unknown-elf-gcc` or `riscv64-elf-gcc`, you may re-use the same compiler for this.

    Note that when compiling userspace programs, you must manually provide a number of command line arguments (such as the location of your compiled libc and any headers).


For [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), we'll rely on a generic `riscv64-elf-gcc` toolchain provided by our distro.

## Creating a Meson cross file

[`mlibc`](https://github.com/managarm/mlibc) uses a build system called [Meson](https://mesonbuild.com/). When cross-compiling, you must tell meson about your compiler and target via a [_cross file_](https://mesonbuild.com/Cross-compilation.html).

For [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), our cross file looks like this:

```toml
{{#include ../../../ci/demo.cross-file}}
```
