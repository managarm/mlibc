# The test relies on libone having an unresolved symbol.
test_additional_link_args += '-Wl,-z,undefs'

libtwo = shared_library('two', 'libtwo.c')
libone = shared_library('one', 'libone.c', link_args: test_additional_link_args)
test_depends = [libone, libtwo]

libtwo_native = shared_library('native-two', 'libtwo.c', native: true)
libone_native = shared_library('native-one', 'libone.c', native: true, link_args: test_additional_link_args)
test_native_depends = [libone_native, libtwo_native]
