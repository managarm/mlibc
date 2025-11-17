# Implementing your sysdeps

## Telling mlibc about your sysdeps

In mlibc's top level `meson.build` file, you'll see a large `if`/`else` chain that selects the right sysdeps subdirectory based on the system name. We'll add ourselves here:
```meson
{{#include ../../../meson.build:demo-sysdeps}}
    # ...
```

Now, create the `sysdeps/demo` folder. At minimum it should contain a `meson.build` file where we'll declare all the `.cpp` files and select header files to install:

```meson
{{#include ../../../sysdeps/demo/meson.build:sources-and-includes}}
```

The `sysdep_supported_options` tells mlibc which 'options' your sysdeps support. For example, the `unistd.h` header is only available when the POSIX option is enabled. 

While `sysdep_supported_options` only states which options are supported to be used by the sysdeps, you are free to disable options with meson options at `meson setup` time. The default behavior is to enable supported options.

## Preparing to build mlibc

Now, from a fresh clone of `mlibc` you can run:

```
$ meson setup --cross-file=path/to/your.cross-file -Ddefault_library=static -Dbuildtype=debug build
```

The `-Ddefault_library=static` tells meson to only produce a statically linked library (`libc.a`). We suggest getting statically linked binaries to work before dynamically linked ones.

Now run `ninja -C build` to start the build. You'll likely get a large number of compilation errors at this point, and we'll work on fixing these in the next sections.

## Selecting ABI headers

Many of the compile errors are because we need to tell mlibc about the ABI between our sysdeps and kernel. For example, we must define the layout of `struct stat`.

We _strongly_ recommend re-using an existing ABI where possible rather than creating your own, because many programs assume a particular ABI and will fail to compile or subtly break if you choose a different one.

For the demo OS, we'll symlink each ABI header to Linux's definition in `abis/linux`. Then, we'll add the following to our `meson.build`:

```meson
{{#include ../../../sysdeps/demo/meson.build:abi-includes}}
```

This is not an exhaustive list of ABI headers but should be enough to get started.

Note that the Linux ABI will be silently changed if it turns out that it is wrong - you end up subscribing to what amounts to ABI breaks if that (hopefully only rarely) happens.

## Implementing (S)crt1

Next, we must provide a definition of the ELF entry point (traditionally named `_start`). Each ISA tends to require its own definition written in assembly; for example, RISC-V targets must initialise the `gp` register before jumping to C++.

Traditionally the file that defines `_start` is called `crt1.S` (or `Scrt1.S` for position independent executables). This produces an object file which has to be linked into every application.

Part of configuring an [OS Specific Toolchain](https://wiki.osdev.org/OS_Specific_Toolchain) is [specifying the location of `(S)crt1.o`](https://wiki.osdev.org/OS_Specific_Toolchain#Start_Files_Directory) so that it is linked automatically. Generic ELF targets must link it explicitly, as the compiler does not know about it.

We recommend copying `(S)crt1.S` from an existing target like Linux. Then, we'll compile it by adding the following to our `meson.build`:

```
{{#include ../../../sysdeps/demo/meson.build:crt1}}
```

## Implementing a C++ entry point

Now, we'll implement the C++ entry point that we call from `crt1.S`. 

```cpp
{{#include ../../../sysdeps/demo/entry.cpp}}
```

The call to `__dlapi_enter` is used to perform initialisation in statically linked executables (but is a no-op in dynamically linked ones). For example, any global constructors in the program will be called from here.

Make sure to take a look at the [ABI specification](https://wiki.osdev.org/System_V_ABI#See_Also) for your architecture for details like proper stack layout. People often forget to implement things like auxiliary vectors which are required by the spec.

## Performing system calls

The final piece of infrastructure we require is the ability to invoke system calls.

For example, on RISC-V a system call is invoked via the `ecall` instruction and requires putting arguments in specific registers, which requires a bit of (inline) assembly. We recommend copying this glue from an existing target.

For the demo OS, this is provided by `syscall.cpp` and `include/bits/syscall.h`.

## Implementing sysdeps

Finally we're ready to implement the actual sysdep functions. For a basic statically-linked hello world program, you'll need to provide definitions of the following sysdep functions:

- `mlibc::sys_libc_panic`
- `mlibc::sys_libc_log`
- `mlibc::sys_isatty`
- `mlibc::sys_write`
- `mlibc::sys_tcb_set`
- `mlibc::sys_anon_allocate`
- `mlibc::sys_anon_free`
- `mlibc::sys_seek`
- `mlibc::sys_exit`
- `mlibc::sys_close`
- `mlibc::sys_futex_wake`
- `mlibc::sys_futex_wait`
- `mlibc::sys_read`
- `mlibc::sys_open`
- `mlibc::sys_vm_map`
- `mlibc::sys_clock_get`

Note that many of these functions are declared as weak symbols. You _must_ include the relevant headers (e.g `<mlibc/all-sysdeps.hpp>`) before providing definitions.

Diverging from the declared sysdep signature will result in different mangling of the function (because they are not declared extern "C", but have C++ linkage instead), which makes it look like the sysdep is missing - this is a silent breakage that likely does not result in compiler errors or even warnings.

Most sysdep functions return an integer error code (0 for success, or a value that matches the sysdep's abi-bits errno definitions on failure) and return data via out parameters. Note that sysdeps shouldn't set errno directly - mlibc will set it from the error code you return.

As a general strategy, it's a good idea to stub whatever's required to make things compile, and then add proper implementations later. For example:

```cpp
{{#include ../../../sysdeps/demo/sysdeps.cpp:stub}}

namespace mlibc {
int sys_close(int fd) { STUB(); }
}
```

## Compiling a test program

At this point, you should be able to compile and link mlibc itself. Congratulations!

Now we'll compile a simple hello world program that we can run on our kernel. If you are using a compiler that targets your OS, compile it like any other C program. As the demo OS is not using one, we will have to tell it about mlibc files manually:
```bash
# Install mlibc to a temporary directory
DESTDIR=/tmp/mlibc-install ninja -C build install

# crtbegin.o and crtend.o are provided by the compiler and 
# -nostdlib stops them from being included. We will have to find their location
CRT_BEGIN=$(${CC} -print-file-name=crtbegin.o)
CRT_END=$(${CC} -print-file-name=crtend.o)

${CC} \
    -static -nostdinc -nostdlib -g \
    -I /tmp/mlibc-install/include \
    /tmp/mlibc-install/lib/crt1.o \
    hello_world_program.c \
    ${CRT_BEGIN} \
    ${CRT_END} \
    /tmp/mlibc-install/lib/libc.a \
    -lgcc \
    -o hello_world_program
```

## Troubleshooting

Something not working? Here are some common issues to look out for:

- Your kernel isn't loading the ELF file correctly. Double check that you're loading the contents from file to the right addresses and zeroing the uninitialised portions.
- Your `sys_anon_allocate` function is broken. Try commenting out `sys_anon_free` and see if that helps. Try printing the addresses to see if you're getting unique allocations.
- You're pushing the arguments/environment/auxiliary vectors to the stack wrong. Double check you're pushing in the right order and everything is properly aligned.
- You're not saving and restoring the userspace state properly when a syscall happens.
- Your `syscall` glue is passing arguments in the wrong registers. Double check that the kernel and userspace agree on the order of arguments.

When stuck, we recommend using GDB to step through the program.

If all else fails, feel free to hop in [the Managarm Discord server](https://discord.gg/7WB6Ur3) and ask for help in `#mlibc-dev`.
