project('freestanding-c-hdrs')

include_dir_path = meson.current_source_dir() / 'include'

# We use `-idirafter` so that clang prefers its own resource headers over these
freestanding_c_hdrs_dep = declare_dependency(
	compile_args: ['-idirafter' + include_dir_path]
)
