lib_fake = shared_library('fake', 'empty.c', install_dir: 'fake')
lib_native_fake = shared_library('native-fake', 'empty.c', native: true, install_dir: 'fake')

test_link_with += [lib_fake]
test_native_link_with += [lib_native_fake]

configure_file(input: 'fake-lib', output: 'libfoo.so', install_dir: 'fake', copy: true)
configure_file(input: 'fake-lib', output: 'libnative-foo.so', install_dir: 'fake', copy: true)
