libfoo = shared_library('foo', 'libfoo.c', c_args: freestanding_hdrs_args, link_args: mlibc_link_args)
test_link_with = [libfoo]

libfoo_native = shared_library('native-foo', 'libfoo.c', native: true, link_args: mlibc_link_args)
test_native_link_with = [libfoo_native]

# The only reference to libfoo is from bar(), which --gc-sections may discard.
# With --as-needed, lld only keeps DT_NEEDED for references that survive that.
test_additional_link_args = ['-Wl,-u,bar']
