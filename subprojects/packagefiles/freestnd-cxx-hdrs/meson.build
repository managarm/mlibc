project('freestnd-cxx-hdrs')

cpu = host_machine.cpu_family()
if cpu == 'x86'
  cpu = 'i686'
endif

incl = include_directories(cpu / 'include', is_system: true)

freestnd_cxx_hdrs_dep = declare_dependency(include_directories: incl)
