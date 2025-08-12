#include <stdint.h>

static uint64_t foo_impl(void) {
    return 69;
}

static uint64_t (*foo_resolver(void))(void) {
    return foo_impl;
}

uint64_t foo(void) __attribute__((ifunc("foo_resolver")));
