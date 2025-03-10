rtld_dso_sources += files(
    'generic/ensure.cpp',
    'generic/file.cpp',
    'generic/fork-exec.cpp',
    'generic/memory.cpp',
)

libc_sources += files(
    'generic/ensure.cpp',
    'generic/entry.cpp',
    'generic/file.cpp',
    'generic/fork-exec.cpp',
    'generic/memory.cpp',
    'generic/time.cpp',
)

if not headers_only
    crtstuff = ['crt0']
    if host_machine.cpu_family() in ['x86_64', 'aarch64']
        crtstuff += [
            'Scrt1',
            'crti',
            'crtn'
        ]
    endif
    foreach crtthing : crtstuff
        crtf = crtthing + '.S'
        crt_src = files(host_machine.cpu_family() / 'crt-src' / crtf)
        crt = custom_target(
            crtthing,
            build_by_default: true,
            command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
            input: crt_src,
            output: crtthing + '.o',
            install: true,
            install_dir: get_option('libdir')
        )
    endforeach

endif
