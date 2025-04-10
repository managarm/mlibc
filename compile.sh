# TARGET = prog
# OBJ = $(TARGET).o
# SRC = $(TARGET).c
# CC = gcc
# CFLAGS = -g
# LDFLAGS = -nostdlib -nostartfiles -static
# GLIBCDIR = /home/my_acct/glibc_install/lib
# STARTFILES = $(GLIBCDIR)/crt1.o $(GLIBCDIR)/crti.o `gcc --print-file-name=crtbegin.o`
# ENDFILES = `gcc --print-file-name=crtend.o` $(GLIBCDIR)/crtn.o
# LIBGROUP = -Wl,--start-group $(GLIBCDIR)/libc.a -lgcc -lgcc_eh -Wl,--end-group

# $(TARGET): $(OBJ)
#         $(CC) $(LDFLAGS) -o $@ $(STARTFILES) $^ $(LIBGROUP) $(ENDFILES) 

# $(OBJ): $(SRC)
#         $(CC) $(CFLAGS) -c $^

# clean:
#         rm -f *.o *.~ $(TARGET)

# aarch64-linux-gnu-gcc -g -c test.c

# aarch64-linux-gnu-gcc -g -nostdlib -nostartfiles -static \
#  -o a.out test.o \
#   ./build/sysdeps/pinceros/Scrt1.o ./build/sysdeps/pinceros/crti.o \
#   -lc -Wl,--start-group ./build/libc.a -Wl,--end-group \
#   ./build/sysdeps/pinceros/crtn.o




# set -eux
# rm -rf tmp
# mkdir tmp
# aarch64-linux-gnu-gcc \
#   -L "./build/" \
#   -Wl,--rpath="./build/" \
#   -Wl,--dynamic-linker="./build/ld.so" \
#   -static \
#   -std=c11 \
#   -o tmp/test_glibc.out \
#   -v \
#   test.c \
# ;


# aarch64-linux-gnu-gcc -static \
#     -nostdlib -nostartfiles \
#     -o a.out \
#     test.c \
#     -L./build/ \
#     -lc -fno-builtin


# aarch64-linux-gnu-gcc -static -nostdlib -nostartfiles \
#     -o a.out test.c \
#     -L./build/install/usr/local/include \
#     -lc -lgcc -Wl,--no-as-needed



# aarch64-linux-gnu-gcc -g -nostdlib -nostartfiles -static \
#     -Xlinker -I./build/ld.so -L./build/install/usr/local/include \
#     -o a.out test.c \
#     ./build/sysdeps/pinceros/Scrt1.o ./build/sysdeps/pinceros/crti.o \
#     ./build/install/usr/local/lib/atomics.o \
#     -L./build/install/usr/local/include \
#     -lc -Wl,--start-group ./build/libc.a -Wl,--end-group \
#     ./build/sysdeps/pinceros/crtn.o


# aarch64-linux-gnu-gcc -v -g -nostdlib -nostartfiles -static \
#     -o a.out test.o \
#     ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
#     -lc  \
#     ./build/sysdeps/pinceros/crtn.o \
#     ./build/install/usr/local/lib/atomics.o
#     # -lc -Wl,--start-group ./build/libc.a -Wl,--end-group \

# aarch64-linux-gnu-gcc -v -g -nostdlib -nostartfiles -static \
#     -o a.out test.o \
#     ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
#     -lc -Wl,--start-group ./build/libc.a -Wl,--end-group \
#     ./build/sysdeps/pinceros/crtn.o \
#     ./build/install/usr/local/lib/atomics.o \
#     ./build/install/usr/local/lib/exceptions.o \
#     ./build/install/usr/local/lib/floats.o \
#     -Wl,-Map=linker.map \
#     -Wl,--no-dynamic-linker \
#     -no-pie

# clang test.c -c --sysroot=/usr/aarch64-linux-gnu

# clang --sysroot=/usr/aarch64-linux-gnu --target=aarch64-none-elf -g -nostdlib -nostartfiles -static \
#     -o a.out test.c\
#     -I./build/install/usr/local/include \
#     ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
#     -Wl,--start-group ./build/libc.a -Wl,--end-group \
#     ./build/sysdeps/pinceros/crtn.o \
#     -Wl,--no-dynamic-linker \
#     -no-pie

# aarch64-linux-gnu-gcc -g -nostdlib -nostartfiles -static -mno-outline-atomics \
#     -o a.out test.c \
#     ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
#     -lc -static-libgcc -lgcc -Wl,--start-group ./build/libc.a -Wl,--end-group \
#     ./build/sysdeps/pinceros/crtn.o \
#     -Wl,-Map=linker.map \
#     -Wl,--no-dynamic-linker \
#     -no-pie

aarch64-linux-gnu-gcc -c test.c -I./build/install/usr/local/include

# aarch64-linux-gnu-ld -nostdlib test.o things.o -L./build/install/usr/local/lib -static \
#     -o a.out \
#     ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
#     -lc /usr/lib/gcc/aarch64-linux-gnu/14.2.0/libgcc.a 

aarch64-linux-gnu-ld -nostdlib test.o -L./build/install/usr/local/lib -static \
    -o a.out \
    ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
    -lc /usr/lib/gcc/aarch64-linux-gnu/14.2.0/libgcc.a 

# specific path to libgcc.a in link flags
# -I and -L