project('freestnd-c-hdrs')

cpu = host_machine.cpu_family()
if cpu == 'x86'
  cpu = 'i686'
endif

incl = include_directories(cpu / 'include')

freestnd_c_hdrs_dep = declare_dependency(include_directories: incl)
