# Implementing your sysdeps (part 1)

As we need libc headers to build the toolchain, we will first implement enough of the sysdeps to install the headers.

## Creating a Meson cross file

mlibc uses a build system called [Meson](https://mesonbuild.com/). When cross-compiling, you must tell meson about your compiler and target via a [_cross file_](https://mesonbuild.com/Cross-compilation.html).

For [`mlibc-demo-os`](https://github.com/64/mlibc-demo-os), our cross file looks like this:

```toml
[host_machine]
system = 'demo'
cpu_family = 'riscv64'
cpu = 'riscv64'
endian = 'little'
[binaries]
c = 'riscv64-demo-gcc'
cpp = 'riscv64-demo-g++'
ar = 'riscv64-demo-ar'
strip = 'riscv64-demo-strip'
ld = 'riscv64-demo-ld'
[built-in options]
c_args = ['-march=rv64gc', '-mabi=lp64d']
cpp_args = ['-march=rv64gc', '-mabi=lp64d']
```

## Telling mlibc about your sysdeps

In mlibc's top level `meson.build` file, you'll see a large `if`/`else` chain that selects the right sysdeps subdirectory based on the system name. We'll add ourselves here:
```meson
{{#include ../../../meson.build:demo-sysdeps}}
    # ...
```

Now, create the `sysdeps/demo` folder. At minimum it should contain a `meson.build` file. Here, all the source and include files are declared for the sysdep. For now, we will only add this:

```meson
{{#include ../../../sysdeps/demo/meson.build:supported-options}}
```

The `sysdep_supported_options` tells mlibc which 'options' your sysdeps support. For example, the `unistd.h` header is only available when the POSIX option is enabled. 

While `sysdep_supported_options` only states which options are supported to be used by the sysdeps, you are free to disable options with meson options at `meson setup` time. The default behavior is to enable supported options.

## Selecting ABI headers

Your userland needs an ABI to communicate with your kernel. For example, we must define the layout of `struct stat`.

Re-using an existing ABI is _strongly_ recommended. Many programs assume a particular ABI and will fail to compile or subtly break if you choose a different one (This is the program's fault and not mlibc's or your sysdeps').

Note that using an existing ABI does _not_ imply you have to implement OS specific features. For example, even if you use the Linux ABI you do not have to implement Linux specific features.

For the demo OS, we'll symlink each ABI header to Linux's definition in `abis/linux`. This is so we don't have to define a whole new ABI for a brief demo. Then, we'll add the following to our `meson.build`:

```meson
{{#include ../../../sysdeps/demo/meson.build:abi-includes}}
```

This is not an exhaustive list of ABI headers but should be enough to get started.

Note that the Linux ABI will be silently changed if it turns out that it is wrong - you end up subscribing to what amounts to ABI breaks if that (hopefully only rarely) happens.

For this reason, it is also _strongly_ recommended to pin the commit or version of mlibc which will be used, as otherwise there could be changes which silently break your compiled software.

If subscribing to the Linux ABI is not desired, you can still fork it by creating a copy under `abis/$os/` and symlinking to it instead of the Linux ABI.

## Configuring mlibc for headers only 

Now, you can run:

```
$ meson \
    setup \
    --cross-file=path/to/your.cross-file \
    --prefix=/usr \
    -Dheaders_only=true \
    headers-build
```

The `-Dheaders_only=true` option tells meson that we only want the headers and not a full libc.

To install the headers into the sysroot, run: 
```
DESTDIR=${SYSROOT_DIR} ninja -C headers-build install
```

We are now ready to start building the toolchain!
