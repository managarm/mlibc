libifunc_test = shared_library('ifunc-test', 'lib.c', dependencies: libc_dep)
test_link_with = [libifunc_test]

libifunc_test_native = shared_library('native-ifunc-test', 'lib.c', native: true)
test_native_link_with = [libifunc_test_native]
