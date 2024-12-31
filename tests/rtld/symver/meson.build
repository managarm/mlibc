version_script = meson.current_source_dir() / 'libfoo.ver'

libfoo = shared_library('foo', 'libfoo.c',
		link_args : ['-Wl,--version-script', version_script])
test_depends = [libfoo]
test_link_with = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true,
		link_args : ['-Wl,--version-script', version_script])
test_native_depends = [libfoo_native]
test_native_link_with = [libfoo_native]
