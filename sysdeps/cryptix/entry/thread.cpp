#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread.hpp>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

namespace mlibc
{
    extern "C" void __mlibc_enter_thread(void* entry, void* user_arg)
    {
        auto tcb = mlibc::get_current_tcb();

        while (!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED))
            mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);
        tcb->invokeThreadFunc(entry, user_arg);

        __atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
        mlibc::sys_futex_wake(&tcb->didExit);
        mlibc::sys_thread_exit();
    }

    static constexpr size_t default_stacksize = 0x200000; // 2 mib
    int sys_prepare_stack(void** stack, void* entry, void* user_arg, void* tcb,
                          size_t* stack_size, size_t* guard_size,
                          void** stack_base)
    {
        (void)tcb;
        if (!*stack_size) *stack_size = default_stacksize;

        uintptr_t map;
        if (*stack)
        {
            map         = reinterpret_cast<uintptr_t>(*stack);
            *guard_size = 0;
        }
        else
        {
            sys_vm_map(nullptr, *stack_size + *guard_size, PROT_NONE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0,
                       reinterpret_cast<void**>(&map));
            if (reinterpret_cast<void*>(map) == MAP_FAILED) return EAGAIN;
            int ret = sys_vm_protect(reinterpret_cast<void*>(map + *guard_size),
                                     *stack_size, PROT_READ | PROT_WRITE);
            if (ret) return EAGAIN;
        }

        *stack_base = reinterpret_cast<void*>(map);
        auto sp = reinterpret_cast<uintptr_t*>(map + *guard_size + *stack_size);
        *--sp   = reinterpret_cast<uintptr_t>(user_arg);
        *--sp   = reinterpret_cast<uintptr_t>(entry);
        *stack  = reinterpret_cast<void*>(sp);
        return 0;
    }
} // namespace mlibc
