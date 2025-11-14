libfoo = shared_library('foo', 'libfoo.c', install_dir: 'real')
libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, install_dir: 'real')

test_link_with += [libfoo]
test_native_link_with += [libfoo_native]
