ansi_sources += files(
	'strcmp.cpp',
	'strlen.cpp',
	'strlen-avx2.S',
	'strlen-sse2.S',
)

arch_defines = [
	'-DMLIBC_ARCH_HAS_STRCMP=1',
	'-DMLIBC_ARCH_HAS_STRLEN=1',
]

libc_cpp_args += arch_defines
static_cpp_args += arch_defines
