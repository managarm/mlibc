libifunc_test = shared_library('ifunc-test', 'lib.c', dependencies: libc_dep, link_args: mlibc_link_args)
test_link_with = [libifunc_test]

libifunc_test_native = shared_library('native-ifunc-test', 'lib.c', native: true, link_args: mlibc_link_args)
test_native_link_with = [libifunc_test_native]
