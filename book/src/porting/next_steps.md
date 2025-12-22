# Next steps

## Dynamic linking

So far you have compiled and run a statically linked executable. If you desire dynamic linking, it is not much more work in mlibc compared to static linking.

To get it to work, you will need to reconfigure mlibc without the `-Ddefault_library=static` option, load *both* the ELF and its interpreter (stored in `PT_INTERP`), and jump to the *interpreter's* entry point.

## Implementing more sysdeps

Most sysdeps in mlibc are defined as weak symbols, and so do not need to be defined right away. Whenever an unimplemented sysdep is hit, mlibc will log about it and return an error to the user application.

The list of sysdeps for every option can be found under its include directory. For example, the sysdeps for the POSIX option are declared in `options/posix/include/mlibc/posix-sysdeps.hpp`. As mentioned earlier, make sure your definitions match the ones in the header, as mlibc won't be able to find them otherwise.

## Enabling more options

For the demo sysdeps, only the POSIX option is enabled. However, there are ports that will need more options enabled. The other toggleable options in mlibc are:

| Option           | Description                                                                                                                                            |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `linux`          | For source compatiblity with Linux programs. Note that this option requires you to provide Linux kernel headers.                                       |
| `linux-epoll`    | For `epoll_*` functions. Included in `linux`, but can be compiled standalone.                                                                          |
| `linux-timerfd`  | For `timerfd_*` functions. Included in `linux`, but can be compiled standalone.                                                                        |
| `linux-signalfd` | For `signalfd_*` functions. Included in `linux`, but can be compiled standalone.                                                                       |
| `linux-eventfd`  | For `eventfd_*` functions. Included in `linux`, but can be compiled standalone.                                                                        |
| `linux-reboot`   | For `reboot*` functions. Included in `linux`, but can be compiled standalone.                                                                          |
| `glibc`          | For glibc-specific extensions like `backtrace` and `getopt_long`. Make sure your gcc port has `gnu-user.h` in its `tm_file` if you enable this option. |
| `bsd`            | for BSD-specific extensions like `openpty` and `getloadavg`                                                                                            |


## Enabling more mlibc features

There are some configuration options which change how mlibc handles things like loading libraries:
- `MLIBC_MAP_DSO_SEGMENTS` - By default, mlibc does not map files directly when loading libraries. This option enables the mapping of libraries in rtld, which is highly recommended if your OS supports mapping files to memory.
- `MLIBC_MMAP_ALLOCATE_DSO` - By default, mlibc will load libraries starting from a preset base memory address. This option makes it so that it will use the addresses returned by the `sys_vm_map` sysdep instead.
- `MLIBC_MAP_FILE_WINDOWS` - Enables the memory mapping of files like `/etc/localtime` when used by mlibc.

To enable them, add them to the root meson.build under your sysdep like so:
```meson
elif host_machine.system() == 'demo'
	internal_conf.set10('CONFIG_NAME', true)
    # ...
```

## Porting software

With mlibc working, you are now ready to start [porting software](https://wiki.osdev.org/Cross-Porting_Software). For your sanity, use a compiler which targets your OS and has a sysroot directory.

If you run into build failures, make sure that you have enabled the options needed by the application as well as defined the ABI correctly. If you truly believe something is an mlibc bug, please reach out to us [on Discord](https://discord.gg/7WB6Ur3) in `#mlibc-dev` or [open an issue](https://github.com/managarm/mlibc/issues/new).

## Upstreaming your port

Once your port is reasonably stable, feel free to submit it to us upstream. This ensures that your sysdeps won't be broken by any internal refactorings.

Though we won't be able to test your kernel on our CI, we require you add your port to the 'Compile sysdeps' GitHub action which checks that compilation succeeds.

It's a good idea to include a `.clang-format` file so that any changes we make to your code will be formatted to your liking.

See the [pull request adding Astral sysdeps](https://github.com/managarm/mlibc/pull/1136) for an example to follow.
