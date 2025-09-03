#include <abi-bits/signal.h>
#include <stddef.h>
#include "context-offsets.h"

// offsets int ucontext_t as used in signals.S
static_assert(offsetof(ucontext_t, uc_mcontext) == UCONTEXT_GREGS_OFFSET);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R8]) == UCONTEXT_OFFSET_R8);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R9]) == UCONTEXT_OFFSET_R9);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R10]) == UCONTEXT_OFFSET_R10);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R11]) == UCONTEXT_OFFSET_R11);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R12]) == UCONTEXT_OFFSET_R12);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R13]) == UCONTEXT_OFFSET_R13);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R14]) == UCONTEXT_OFFSET_R14);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_R15]) == UCONTEXT_OFFSET_R15);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RDI]) == UCONTEXT_OFFSET_RDI);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RSI]) == UCONTEXT_OFFSET_RSI);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RBP]) == UCONTEXT_OFFSET_RBP);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RBX]) == UCONTEXT_OFFSET_RBX);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RDX]) == UCONTEXT_OFFSET_RDX);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RAX]) == UCONTEXT_OFFSET_RAX);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RCX]) == UCONTEXT_OFFSET_RCX);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RSP]) == UCONTEXT_OFFSET_RSP);
static_assert(offsetof(ucontext_t, uc_mcontext.gregs[REG_RIP]) == UCONTEXT_OFFSET_RIP);
