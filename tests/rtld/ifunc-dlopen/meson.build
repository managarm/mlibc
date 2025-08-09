libifunc_test = shared_library('ifunc-test-dlopen', 'lib.c', dependencies: libc_dep, install: true)
test_depends = [libifunc_test]

libifunc_test_native = shared_library('native-ifunc-test-dlopen', 'lib.c', native: true)
test_native_depends = [libifunc_test_native]
