libfoo = shared_library('foo', 'libfoo.c')
test_depends = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true)
test_native_depends = [libfoo_native]
