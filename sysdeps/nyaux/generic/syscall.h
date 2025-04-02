#pragma once
#include <stdint.h>
struct __syscall_ret {
  uint64_t ret;
  uint64_t err;
};

static __syscall_ret __syscall6(int number, uint64_t arg1, uint64_t arg2,
                                uint64_t arg3, uint64_t arg4, uint64_t arg5,
                                uint64_t arg6) {
  struct __syscall_ret ret;
  register uint64_t arg_r8 asm("r8") = arg4;
  register uint64_t arg_r9 asm("r9") = arg5;
  register uint64_t arg_r10 asm("r10") = arg6;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg_r8),
                 "r"(arg_r9), "r"(arg_r10)
               : "rcx", "r11", "memory");
  return ret;
}
static __syscall_ret __syscall5(int number, uint64_t arg1, uint64_t arg2,
                                uint64_t arg3, uint64_t arg4, uint64_t arg5) {
  struct __syscall_ret ret;
  register uint64_t arg_r8 asm("r8") = arg4;
  register uint64_t arg_r9 asm("r9") = arg5;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg_r8),
                 "r"(arg_r9)
               : "rcx", "r11", "memory");
  return ret;
}
static __syscall_ret __syscall4(int number, uint64_t arg1, uint64_t arg2,
                                uint64_t arg3, uint64_t arg4) {
  struct __syscall_ret ret;
  register uint64_t arg_r8 asm("r8") = arg4;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg_r8)
               : "rcx", "r11", "memory");
  return ret;
}
static __syscall_ret __syscall3(int number, uint64_t arg1, uint64_t arg2,
                                uint64_t arg3) {
  struct __syscall_ret ret;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number), "D"(arg1), "S"(arg2), "d"(arg3)
               : "rcx", "r11", "memory");
  return ret;
}
static __syscall_ret __syscall2(int number, uint64_t arg1, uint64_t arg2) {
  struct __syscall_ret ret;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number), "D"(arg1), "S"(arg2)
               : "rcx", "r11", "memory");
  return ret;
}
static __syscall_ret __syscall1(int number, uint64_t arg1) {
  struct __syscall_ret ret;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number), "D"(arg1)
               : "rcx", "r11", "memory");
  return ret;
}
static __syscall_ret __syscall0(int number) {
  struct __syscall_ret ret;
  asm volatile("syscall"
               : "=a"(ret.ret), "=d"(ret.err)
               : "a"(number)
               : "rcx", "r11", "memory");
  return ret;
}