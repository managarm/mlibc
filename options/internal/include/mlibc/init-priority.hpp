#ifndef MLIBC_INIT_PRIORITY_HPP
#define MLIBC_INIT_PRIORITY_HPP

// Priorities 0-100 are reserved for the C library implementation; 101 is the
// lowest a program may use. In a static build mlibc's .init_array entries share
// the array with the executable's, so mlibc needs its own to run first.

// Startup data: environ, program_invocation_name, ...
#define MLIBC_INIT_PRIORITY_STARTUP 50

// Standard streams, after the environment is populated.
#define MLIBC_INIT_PRIORITY_STDIO 51

#endif // MLIBC_INIT_PRIORITY_HPP
