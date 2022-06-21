#ifndef _LINUX_CPU_SET_H
#define _LINUX_CPU_SET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long __cpu_mask;

#define __CPU_SETSIZE	1024
#define __NCPUBITS	(8 * sizeof (__cpu_mask))

#define __CPUELT(cpu)	((cpu) / __NCPUBITS)
#define __CPUMASK(cpu)	((__cpu_mask) 1 << ((cpu) % __NCPUBITS))

# define __CPU_ZERO_S(setsize, cpusetp) \
  do __builtin_memset (cpusetp, '\0', setsize); while (0)
#define __CPU_SET_S(cpu, setsize, cpusetp) \
  (__extension__							      \
   ({ size_t __cpu = (cpu);						      \
      __cpu / 8 < (setsize)						      \
      ? (((__cpu_mask *) ((cpusetp)->__bits))[__CPUELT (__cpu)]		      \
	 |= __CPUMASK (__cpu))						      \
      : 0; }))
#define __CPU_ISSET_S(cpu, setsize, cpusetp) \
  (__extension__							      \
   ({ size_t __cpu = (cpu);						      \
      __cpu / 8 < (setsize)						      \
      ? ((((const __cpu_mask *) ((cpusetp)->__bits))[__CPUELT (__cpu)]	      \
	  & __CPUMASK (__cpu))) != 0					      \
      : 0; }))

// typedef struct
// {
//   __cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
// } cpu_set_t;


#ifdef __cplusplus
}
#endif

#endif // _LINUX_CPU_SET_H
