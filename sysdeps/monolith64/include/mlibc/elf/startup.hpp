#pragma once

namespace mlibc::elf {

struct StartupInfo {
    void *stack_start;
    void *stack_end;
    int argc;
    char **argv;
};

extern "C" void start(StartupInfo *info);

} // namespace mlibc::elf

