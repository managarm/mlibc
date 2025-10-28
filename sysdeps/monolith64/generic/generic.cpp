#include <stddef.h>
#include <stdint.h>
#include <mlibc/posix-sysdeps.hpp>

namespace mlibc {

// --- syscall numbers for your OS ---
constexpr uint64_t SYS_WRITE = 1;
constexpr uint64_t SYS_READ  = 2;
constexpr uint64_t SYS_EXIT  = 3;

// --- write to file descriptor ---
ssize_t sys_write(int fd, const void *buf, size_t count) {
    uint64_t ret;
    asm volatile (
        "mov %1, %%rax\n\t"  // syscall number
        "mov %2, %%rdi\n\t"  // fd
        "mov %3, %%rsi\n\t"  // buf
        "mov %4, %%rdx\n\t"  // count
        "int $0x69\n\t"
        "mov %%rax, %0\n\t"
        : "=r"(ret)
        : "r"(SYS_WRITE), "r"((uint64_t)fd), "r"(buf), "r"(count)
        : "rax", "rdi", "rsi", "rdx", "memory"
    );
    return (ssize_t)ret;
}

// --- read from file descriptor ---
ssize_t sys_read(int fd, void *buf, size_t count) {
    uint64_t ret;
    asm volatile (
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rsi\n\t"
        "mov %4, %%rdx\n\t"
        "int $0x69\n\t"
        "mov %%rax, %0\n\t"
        : "=r"(ret)
        : "r"(SYS_READ), "r"((uint64_t)fd), "r"(buf), "r"(count)
        : "rax", "rdi", "rsi", "rdx", "memory"
    );
    return (ssize_t)ret;
}

// --- exit process ---
[[noreturn]] void sys_exit(int status) {
    asm volatile (
        "mov %0, %%rax\n\t"
        "mov %1, %%rdi\n\t"
        "int $0x69\n\t"
        :
        : "r"(SYS_EXIT), "r"((uint64_t)status)
        : "rax", "rdi", "memory"
    );
    for (;;) {} // fallback
}

// --- optional: write directly to framebuffer ---
void fb_put_pixel(uint64_t x, uint64_t y, uint32_t color) {
    constexpr uint64_t FB_ADDR = 0xFB0000;
    constexpr uint64_t WIDTH   = 1024; // adjust to your framebuffer width
    uint32_t *fb = (uint32_t*)FB_ADDR;
    fb[y * WIDTH + x] = color;
}

} // namespace mlibc

