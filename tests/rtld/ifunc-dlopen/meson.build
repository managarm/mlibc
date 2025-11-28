libifunc_test = shared_library('ifunc-test-dlopen', 'lib.c', dependencies: libc_dep, install: true, link_args: mlibc_link_args)
test_depends = [libifunc_test]

libifunc_test_native = shared_library('native-ifunc-test-dlopen', 'lib.c', native: true, link_args: mlibc_link_args)
test_native_depends = [libifunc_test_native]
