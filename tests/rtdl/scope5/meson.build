libfoo = shared_library('foo', 'libfoo.c')
test_link_with = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true)
test_native_link_with = [libfoo_native]
