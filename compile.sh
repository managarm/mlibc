aarch64-linux-gnu-gcc -c test.c -I./build/install/usr/local/include -DPINCEROS

aarch64-linux-gnu-ld -nostdlib test.o -L./build/install/usr/local/lib -static --no-omagic \
    -o a.out \
    ./build/sysdeps/pinceros/crt0.o ./build/sysdeps/pinceros/crti.o \
    -lc /usr/lib/gcc/aarch64-linux-gnu/14.2.0/libgcc.a \
    -T script.ld \
    -e _start
