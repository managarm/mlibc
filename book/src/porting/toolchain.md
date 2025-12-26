# Building the toolchain

To compile [`mlibc`](https://github.com/managarm/mlibc) and any userspace programs which link against it, you'll need a suitable compiler. For that, you will need to build a full [OS Specific Toolchain](https://wiki.osdev.org/OS_Specific_Toolchain). Using your host's compiler or a generic toolchain could lead into strange build failures and is not recommended nor supported.

For [mlibc-demo-os](https://github.com/managarm/mlibc-demo-os), the `riscv64-demo-gcc` toolchain is used. The patches (with comments) to add support for it in Binutils and GCC are available [here](https://github.com/managarm/mlibc-demo-os/tree/main/user/patches). The patches for architectures other than riscv64 will differ, so keep that in mind.

The first step is downloading the sources for [Binutils](https://ftpmirror.gnu.org/binutils/) and [GCC](https://ftpmirror.gnu.org/gcc/), then applying your patches. The versions used in this guide are Binutils 2.45.1 and GCC 15.2.0.

## Building Binutils

As GCC depends on a binutils for the target, we will build binutils first:
```bash
cd binutils-2.45.1
mkdir build
cd build

# --with-sysroot will tell the linker where to search for libraries.
# --enable-default-execstack=no will tell the linker to 
#   not use an executable stack by default.
../configure \
    --target=riscv64-demo \
    --prefix=/usr \
    --with-sysroot="${SYSROOT_DIR}" \
    --disable-werror \
    --enable-default-execstack=no

make -j$(nproc)

DESTDIR="${TOOLCHAIN_DIR}" make install
```

**Make sure you have `${TOOLCHAIN_DIR}/usr/bin` in your `$PATH`.** If you don't, you *WILL* run into issues when building GCC.

## Building GCC
Now, build GCC:
```
cd gcc-15.2.0
mkdir build
cd build

# --with-sysroot will tell the compiler where to search for the libc headers 
#   during GCC compilation, as well as include dirs and libraries when compiling.
# --enable-languages=c,c++ will tell it to only build the C and C++ compilers
# --enable-threads=posix enables pthread support
# --disable-multilib disables building a multilib gcc
    CFLAGS_FOR_TARGET="-march=rv64gc -mabi-lp64d" \
    CXXFLAGS_FOR_TARGET="-march=rv64gc -mabi-lp64d" \
../configure \
    --target=riscv64-demo \
    --prefix=/usr \
    --with-sysroot="${SYSROOT_DIR}" \
    --enable-languages=c,c++ \
    --enable-threads=posix \
    --disable-multilib \
    --enable-shared \
    --enable-host-shared

make -j$(nproc) all-gcc all-target-libgcc
DESTDIR="${TOOLCHAIN_DIR}" make install-gcc install-target-libgcc
```

After that, you'll have a compiler that targets your OS and implicitly links against mlibc! However, if you try to compile a simple program using it, you will run into errors:
```
$ riscv64-demo-gcc -march=rv64gc -mabi=lp64d helloworld.c
ld: cannot find crt1.o: No such file or directory
ld: cannot find -lc: No such file or directory
```

This is because we still have not built mlibc, and so it cannot find needed libc files.
