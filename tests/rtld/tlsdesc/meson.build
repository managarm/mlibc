tls_flags = []
tls_flags_native = []

if host_machine.cpu_family() in ['x86', 'x86_64']
	tls_flags = ['-mtls-dialect=gnu2']
	tls_flags_native = tls_flags
elif host_machine.cpu_family() in ['aarch64']
	if meson.get_compiler('c', native: false).has_argument('-mtls-dialect=gnu2')
		tls_flags = ['-mtls-dialect=gnu2']
	else
		tls_flags = ['-mtls-dialect=desc']
	endif
	if meson.get_compiler('c', native: true).has_argument('-mtls-dialect=gnu2')
		tls_flags_native = ['-mtls-dialect=gnu2']
	else
		tls_flags_native = ['-mtls-dialect=desc']
	endif
endif

libfoo = shared_library('foo', 'libfoo.c', c_args: tls_flags, link_args: mlibc_link_args + tls_flags)
test_depends = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, c_args: tls_flags_native, link_args: mlibc_link_args + tls_flags_native)
test_native_depends = [libfoo_native]
