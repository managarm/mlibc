if [ "$1" = "--install" ]; then
  meson setup -Dglibc_option=enabled -Dbsd_option=enabled -Ddefault_library=static build-monolith64 --reconfigure
fi
meson install -C build-monolith64 --destdir=./install

gcc -c test.c -I./build/install/usr/local/include
ld -nostdlib test.o -L./build-monolith64/install/usr/local/lib -static -o a.out ./build-monolith64/sysdeps/monolith64/crt0.o ./build-monolith64/sysdeps/monolith64/crti.o -lc /usr/lib/gcc/x86_64-pc-linux-gnu/14.3.1/libgcc.a
