project('freestnd-c-hdrs')

cpu = host_machine.cpu_family()
if cpu == 'x86'
  cpu = 'i686'
endif

include_dir_path = meson.current_source_dir() / cpu / 'include'

# We use `-idirafter` so that clang prefers its own resource headers over these
freestnd_c_hdrs_dep = declare_dependency(
	compile_args: ['-idirafter' + include_dir_path]
)
