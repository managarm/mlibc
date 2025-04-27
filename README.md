# mlibc

mlibc is a fully featured C standard library designed with portability in mind.

We support a number of architectures (x86-64, AArch64, RISC-V, IA-32, m68k, LoongArch64), and provide a clean syscall abstraction layer for new operating system ports to plug into.

Unlike other portable C standard libraries like newlib, we aim for feature parity with glibc/musl, i.e full pthread support and GNU extensions.

mlibc is capable enough to run a range of software, including Xorg, several Wayland compositors, Mesa, and web browsers like WebKitGTK -- though some features are still missing.

Individual operating systems can opt in or out of certain features as desired; for example POSIX APIs like `pthread` are gated behind the POSIX 'option', Linux APIs like `epoll` are gated behind the Linux option, etc.

![Continuous Integration](https://github.com/managarm/mlibc/workflows/Continuous%20Integration/badge.svg)

**Official Discord server:** https://discord.gg/7WB6Ur3

**AUR package** (provides `mlibc-gcc`): https://aur.archlinux.org/packages/mlibc

## Design of the library

| Directory | Purpose |
| --- | --- |
| `options/` | (More or less) OS-independent headers and code.<br>`options/` is divided into subdirectories that can be enabled or disabled by ports.|
| `sysdeps/` | OS-specific headers and code.<br>`sysdeps/` is divided into per-port subdirectories. Exactly one of those subdirectories is enabled in each build.|
| `abis/` | OS-specific interface headers ("ABI headers"). Those contain the constants and structs of the OS interface. For example, the numerical values of `SEEK_SET` or `O_CREAT` live here, as well as structs like `struct stat`. ABI headers are _only_ allowed to contain constants, structs and unions but _no_ function declarations or logic.<br>`abis/` is divided into per-OS subdirectories but this division is for organizational purposes only. Ports can still mix headers from different `abis/` subdirectories.|

## Porting mlibc to a new OS

Ports to new OSes are welcome. To port mlibc to another OS, the following changes need to be made:
1. Add new `sysdeps/` subdirectory `sysdeps/some-new-os/` and a `meson.build` to compile it. Integrate `sysdeps/some-new-os/meson.build` into the toplevel `meson.build`.
2. Create ABI headers in `abis/some-new-os/`. Add symlinks in `sysdeps/some-new-os/include/abi-bits` to your ABI headers. Look at existing ports to figure out the ABI headers required for the options enabled by `sysdeps/some-new-os/meson.build`.
3. In `sysdeps/some-new-os/`, add code to implement (a subset of) the functions from `options/internal/include/mlibc/internal-sysdeps.hpp`. Which subset you need depends on the options that `sysdeps/some-new-os/meson.build` enables.

We recommend that new ports do not build from `master` as we occasionally make internal changes that cause out-of-tree sysdeps to break. Instead we recommend you pin a specific release (or commit), or to upstream your changes to this repository so that we can build them on our CI and thus any breakages will be fixed by us in-tree.

## Build Configuration

The following custom meson options are accepted, in addition to the [built-in options](https://mesonbuild.com/Builtin-options.html). The options below are booleans which default to false (see `meson_options.txt`).

- `headers_only`: Only install headers; don't build `libc.so` or `ld.so`.
- `no_headers`: Don't install headers; only build `libc.so` and `ld.so`.
- `build_tests`: Build the test suite (see below).
- `x_option`: Enable `x` component of mlibc functionality. See `meson_options.txt` for a full list of possible values for `x`. This may be used to e.g disable POSIX and glibc extensions.
- `linux_kernel_headers`: Allows for directing mlibc to installed linux headers. [These can be obtained easily](https://docs.kernel.org/kbuild/headers_install.html), placed in a directory and this option set to the corresponding path. This is required if the linux option is enabled, i.e. when the linux option is not disabled.
- `debug_allocator`: Replace the normal allocator with a debug allocator (see `mlibc/options/internal/generic/allocator.cpp` for implementation details).
- `use_freestnd_hdrs`: Use freestnd-c{,xx}-hdrs instead of looking for compiler headers. Useful if not using a compiler with the correct target triple.

The type of library to be built (static, shared, or both) is controlled by meson's `default_library` option. Passing `-Ddefault_library=static` effectively disables the dynamic linker.

We also support building with `-Db_sanitize=undefined` to use UBSan inside mlibc. Note that this does not enable UBSan for external applications which link against `libc.so`, but it can be useful during development to detect internal bugs (e.g when adding new sysdeps).

## Running pre-commit hooks

To format your code before submitting a PR, you should install [`pre-commit`](https://pre-commit.com/). Then do `pre-commit install` to install the git hook which runs each time you commit. Alternatively, you can do `pre-commit run -a` to manually format all files.

## Running Tests

The `mlibc` test suite can be run under a Linux host. To do this, first install a set of Linux kernel headers (as described [here](https://docs.kernel.org/kbuild/headers_install.html)).

A convenience script is provided to download and install the Linux kernel headers into a local `linux-headers` directory:
```
./scripts/get-linux-headers.sh
```
Then run from the project root:
```
meson setup -Dbuild_tests=true -Dlinux_kernel_headers=./linux-headers build
```
This will create a `build` directory. Then, `cd build` and run the tests (showing output) with:
```
meson test -v
```
