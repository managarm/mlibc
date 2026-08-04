# WallOS Sysdep

This is based entirely off of the "demo" sysdep, except we target x86_64.

I have my own `wallos/syscall.h` file (which if I remember correctly, is based on the old LemonOS one).
This is likely to change eventually.

The `crt*` files are incredibly generic, but are my own.

## Compile Commands

From project root:

- For full build:
  - `meson setup --cross=wallos-crossfile.txt build-mlibc --prefix=/wallos/sysroot/ -Ddefault_library=static`
- For header only build:
  - `meson setup build-mlibc --prefix=/wallos/sysroot/ -Ddefault_library=static -Dheaders_only=true`

You can pass `--wipe` to meson after the header only build as required.

## Full Build Info

There are a few files that require a small addition.
This may or may not change based on upstream changes.

Add this:
```c
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
```

To the tops of these files:

- `options/ansi/generic/locale.cpp`
- `options/ansi/generic/stdio.cpp`
- `options/ansi/generic/time.cpp`
- `options/internal/generic/locale-defaults.cpp`
- `options/internal/generic/locale.cpp`
