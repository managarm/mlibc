sysdep_supported_options = {
    'posix': true,
    'linux': false,
    'glibc': true,
    'bsd': true,
}

# Files that implement runtime + basic sysdeps
'''
rtld_sources += files(
    'generic/generic.cpp',
)

libc_sources += files(
    'generic/entry.cpp',
    'generic/generic.cpp',
)
'''
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
# Install only the minimal ABI headers required by mlibc.
# You can prune this list as your OS evolves — these are the essentials.
'''
if not no_headers
    install_headers(
        'include/abi-bits/errno.h',
        'include/abi-bits/fcntl.h',
        'include/abi-bits/stat.h',
        'include/abi-bits/signal.h',
        'include/abi-bits/time.h',
        'include/abi-bits/resource.h',
        'include/abi-bits/mode_t.h',
        'include/abi-bits/uid_t.h',
        'include/abi-bits/gid_t.h',
        'include/abi-bits/pid_t.h',
        subdir: 'abi-bits',
        follow_symlinks: true
    )

    install_headers(
        'include/sys/sysmacros.h',
        subdir: 'sys',
    )
endif
'''
# Build and install minimal C runtime objects.
if not headers_only
    crt = custom_target('crt0',
        build_by_default: true,
        command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
        input: host_machine.cpu_family() / 'crt-src/crt0.S',
        output: 'crt0.o',
        install: true,
        install_dir: get_option('libdir')
    )

    custom_target('crti',
        build_by_default: true,
        command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
        input: host_machine.cpu_family() / 'crt-src/crti.S',
        output: 'crti.o',
        install: true,
        install_dir: get_option('libdir')
    )

    custom_target('crtn',
        build_by_default: true,
        command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
        input: host_machine.cpu_family() / 'crt-src/crtn.S',
        output: 'crtn.o',
        install: true,
        install_dir: get_option('libdir')
    )
endif

