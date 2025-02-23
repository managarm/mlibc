#ifndef _LINUX_KVM_H
#define _LINUX_KVM_H

#include <stddef.h>

// KVM version supported by Ironclad.
#define KVM_API_VERSION 12

// System ioctls (to /dev/kvm).
#define KVM_GET_API_VERSION             1
#define KVM_CREATE_VM                   2
#define KVM_GET_MSR_INDEX_LIST          3
#define KVM_CHECK_EXTENSION             4
#define KVM_GET_VCPU_MMAP_SIZE          5
#define KVM_MEMORY_ENCRYPT_REG_REGION   6
#define KVM_MEMORY_ENCRYPT_UNREG_REGION 7

// VM ioctls.
#define KVM_CREATE_VCPU            8
#define KVM_GET_DIRTY_LOG          9
#define KVM_MEMORY_ENCRYPT_OP      10
#define KVM_SET_USER_MEMORY_REGION 11

// VCPU ioctls.
#define KVM_RUN             12
#define KVM_GET_REGS        13
#define KVM_SET_REGS        14
#define KVM_GET_SREGS       15
#define KVM_SET_SREGS       16
#define KVM_TRANSLATE       17
#define KVM_INTERRUPT       18
#define KVM_GET_MSRS        19
#define KVM_SET_MSRS        20
#define KVM_SET_CPUID       21
#define KVM_SET_SIGNAL_MASK 22
#define KVM_GET_FPU         23
#define KVM_SET_FPU         24

// KVM extensions the kernel supports that can be querried with
// KVM_CHECK_EXTENSION.
#define KVM_CAP_USER_MEMORY 1
#define KVM_CAP_NR_VCPUS    2

// Structure to get from KVM_GET_MSR_INDEX_LIST.
struct kvm_msr_list {
   uint32_t nmsrs; /* number of msrs in entries */
   uint32_t indices[0];
} __attribute__((packed));

// Structure to use for KVM_MEMORY_ENCRYPT_REG_REGION
struct kvm_enc_region {
   uint64_t addr;
   uint64_t size;
} __attribute__((packed));

// Structure for KVM_GET_DIRTY_LOG.
struct kvm_dirty_log {
   uint32_t slot;
   uint32_t padding;
   void *dirty_bitmap;
} __attribute__((packed));

// Memory structure to pass to KVM_SET_USER_MEMORY_REGION.
#define KVM_MEM_READONLY (1 << 1)
struct kvm_userspace_mem_region {
   uint32_t slot;
   uint32_t flags;
   uint64_t guest_phys_addr;
   uint64_t memory_size;
   uint64_t userspace_addr;
} __attribute__((packed));

// Structure for x86's KVM_GET_REGS/KVM_SET_REGS
struct kvm_regs {
   uint64_t rax, rbx, rcx, rdx;
   uint64_t rsi, rdi, rsp, rbp;
   uint64_t r8,  r9,  r10, r11;
   uint64_t r12, r13, r14, r15;
   uint64_t rip, rflags;
} __attribute__((packed));

// Structure for KVM_TRANSLATE.
struct kvm_translation {
   uint64_t linear_address;
   uint64_t physical_address;
   uint8_t  valid;
   uint8_t  writeable;
   uint8_t  usermode;
   uint8_t  pad[5];
} __attribute__((packed));

// Structure for KVM_INTERRUPT.
struct kvm_interrupt {
   uint32_t irq;
} __attribute__((packed));

// Structure for KVM_GET_MSRS/KVM_SET_MSRS.
struct kvm_msrs {
   uint32_t nmsrs;
   struct kvm_msr_entry entries[0];
} __attribute__((packed));

struct kvm_msr_entry {
   uint32_t index;
   uint32_t reserved;
   uint64_t data;
} __attribute__((packed));

// Structure for KVM_SET_CPUID.
struct kvm_cpuid_entry {
   uint32_t function;
   uint32_t eax;
   uint32_t ebx;
   uint32_t ecx;
   uint32_t edx;
   uint32_t padding;
} __attribute__((packed));

struct kvm_cpuid {
   uint32_t nent;
   uint32_t padding;
   struct kvm_cpuid_entry entries[0];
} __attribute__((packed));

// Structure for KVM_SET_SIGNAL_MASK.
struct kvm_signal_mask {
   uint32_t len;
   uint8_t sigset[0];
} __attribute__((packed));

// Structure for x86's KVM_GET_FPU / KVM_SET_FPU.
struct kvm_fpu {
   uint8_t  fpr[8][16];
   uint16_t fcw;
   uint16_t fsw;
   uint8_t  ftwx;  /* in fxsave format */
   uint8_t  pad1;
   uint16_t last_opcode;
   uint64_t last_ip;
   uint64_t last_dp;
   uint8_t  xmm[16][16];
   uint32_t mxcsr;
   uint32_t pad2;
} __attribute__((packed));

#endif /* _LINUX_KVM_H */
