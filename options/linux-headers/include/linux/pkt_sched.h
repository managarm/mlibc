#ifndef _LINUX_PKT_SCHED_H
#define _LINUX_PKT_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/types.h>
#include <stdint.h>

struct tc_ratespec {
	unsigned char cell_log;
	uint8_t linklayer;
	unsigned short	overhead;
	short cell_align;
	unsigned short	mpu;
	uint32_t rate;
};

#define TC_RTAB_SIZE	1024

struct tc_tbf_qopt {
	struct tc_ratespec rate;
	struct tc_ratespec peakrate;
	uint32_t limit;
	uint32_t buffer;
	uint32_t mtu;
};

#define MAX_DPs 16

enum {
   TCA_GRED_UNSPEC,
   TCA_GRED_PARMS,
   TCA_GRED_STAB,
   TCA_GRED_DPS,
   TCA_GRED_MAX_P,
   TCA_GRED_LIMIT,
   TCA_GRED_VQ_LIST,
   __TCA_GRED_MAX,
};

#define TCA_GRED_MAX (__TCA_GRED_MAX - 1)

struct tc_gred_sopt {
	uint32_t DPs;
	uint32_t def_DP;
	uint8_t grio;
	uint8_t flags;
	uint16_t pad1;
};

enum {
	TCA_DRR_UNSPEC,
	TCA_DRR_QUANTUM,
	__TCA_DRR_MAX
};

#define TCA_DRR_MAX (__TCA_DRR_MAX - 1)

enum {
	TCA_CODEL_UNSPEC,
	TCA_CODEL_TARGET,
	TCA_CODEL_LIMIT,
	TCA_CODEL_INTERVAL,
	TCA_CODEL_ECN,
	TCA_CODEL_CE_THRESHOLD,
	__TCA_CODEL_MAX
};

#define TCA_CODEL_MAX (__TCA_CODEL_MAX - 1)

#define TC_HTB_NUMPRIO	8
#define TC_HTB_MAXDEPTH	8
#define TC_HTB_PROTOVER	3

struct tc_htb_opt {
	struct tc_ratespec rate;
	struct tc_ratespec ceil;
	uint32_t buffer;
	uint32_t cbuffer;
	uint32_t quantum;
	uint32_t level;
	uint32_t prio;
};

struct tc_htb_glob {
	uint32_t version;
	uint32_t rate2quantum;
	uint32_t defcls;
	uint32_t debug;
	uint32_t direct_pkts;
};

enum {
	TCA_HTB_UNSPEC,
	TCA_HTB_PARMS,
	TCA_HTB_INIT,
	TCA_HTB_CTAB,
	TCA_HTB_RTAB,
	TCA_HTB_DIRECT_QLEN,
	TCA_HTB_RATE64,
	TCA_HTB_CEIL64,
	TCA_HTB_PAD,
	TCA_HTB_OFFLOAD,
	__TCA_HTB_MAX,
};

#define TCA_HTB_MAX (__TCA_HTB_MAX - 1)

enum {
	TCA_TBF_UNSPEC,
	TCA_TBF_PARMS,
	TCA_TBF_RTAB,
	TCA_TBF_PTAB,
	TCA_TBF_RATE64,
	TCA_TBF_PRATE64,
	TCA_TBF_BURST,
	TCA_TBF_PBURST,
	TCA_TBF_PAD,
	__TCA_TBF_MAX,
};

#define TCA_TBF_MAX (__TCA_TBF_MAX - 1)

enum {
	TCA_SFB_UNSPEC,
	TCA_SFB_PARMS,
	__TCA_SFB_MAX,
};

#define TCA_SFB_MAX (__TCA_SFB_MAX - 1)

struct tc_sfb_qopt {
	uint32_t rehash_interval;
	uint32_t warmup_time;
	uint32_t max;
	uint32_t bin_size;
	uint32_t increment;
	uint32_t decrement;
	uint32_t limit;
	uint32_t penalty_rate;
	uint32_t penalty_burst;
};

enum {
	TCA_FQ_UNSPEC,
	TCA_FQ_PLIMIT,
	TCA_FQ_FLOW_PLIMIT,
	TCA_FQ_QUANTUM,
	TCA_FQ_INITIAL_QUANTUM,
	TCA_FQ_RATE_ENABLE,
	TCA_FQ_FLOW_DEFAULT_RATE,
	TCA_FQ_FLOW_MAX_RATE,
	TCA_FQ_BUCKETS_LOG,
	TCA_FQ_FLOW_REFILL_DELAY,
	TCA_FQ_ORPHAN_MASK,
	TCA_FQ_LOW_RATE_THRESHOLD,
	TCA_FQ_CE_THRESHOLD,
	TCA_FQ_TIMER_SLACK,
	TCA_FQ_HORIZON,
	TCA_FQ_HORIZON_DROP,
	__TCA_FQ_MAX
};

#define TCA_FQ_MAX (__TCA_FQ_MAX - 1)

enum {
	TCA_QFQ_UNSPEC,
	TCA_QFQ_WEIGHT,
	TCA_QFQ_LMAX,
	__TCA_QFQ_MAX
};

#define TCA_QFQ_MAX (__TCA_QFQ_MAX - 1)

enum {
	TCA_CAKE_UNSPEC,
	TCA_CAKE_PAD,
	TCA_CAKE_BASE_RATE64,
	TCA_CAKE_DIFFSERV_MODE,
	TCA_CAKE_ATM,
	TCA_CAKE_FLOW_MODE,
	TCA_CAKE_OVERHEAD,
	TCA_CAKE_RTT,
	TCA_CAKE_TARGET,
	TCA_CAKE_AUTORATE,
	TCA_CAKE_MEMORY,
	TCA_CAKE_NAT,
	TCA_CAKE_RAW,
	TCA_CAKE_WASH,
	TCA_CAKE_MPU,
	TCA_CAKE_INGRESS,
	TCA_CAKE_ACK_FILTER,
	TCA_CAKE_SPLIT_GSO,
	TCA_CAKE_FWMARK,
	__TCA_CAKE_MAX
};
#define TCA_CAKE_MAX (__TCA_CAKE_MAX - 1)

enum {
	TCA_PIE_UNSPEC,
	TCA_PIE_TARGET,
	TCA_PIE_LIMIT,
	TCA_PIE_TUPDATE,
	TCA_PIE_ALPHA,
	TCA_PIE_BETA,
	TCA_PIE_ECN,
	TCA_PIE_BYTEMODE,
	TCA_PIE_DQ_RATE_ESTIMATOR,
	__TCA_PIE_MAX
};
#define TCA_PIE_MAX (__TCA_PIE_MAX - 1)

enum {
	TCA_FQ_PIE_UNSPEC,
	TCA_FQ_PIE_LIMIT,
	TCA_FQ_PIE_FLOWS,
	TCA_FQ_PIE_TARGET,
	TCA_FQ_PIE_TUPDATE,
	TCA_FQ_PIE_ALPHA,
	TCA_FQ_PIE_BETA,
	TCA_FQ_PIE_QUANTUM,
	TCA_FQ_PIE_MEMORY_LIMIT,
	TCA_FQ_PIE_ECN_PROB,
	TCA_FQ_PIE_ECN,
	TCA_FQ_PIE_BYTEMODE,
	TCA_FQ_PIE_DQ_RATE_ESTIMATOR,
	__TCA_FQ_PIE_MAX
};
#define TCA_FQ_PIE_MAX (__TCA_FQ_PIE_MAX - 1)

#define FQ_CODEL_QUANTUM_MAX (1 << 20)

enum {
	TCA_FQ_CODEL_UNSPEC,
	TCA_FQ_CODEL_TARGET,
	TCA_FQ_CODEL_LIMIT,
	TCA_FQ_CODEL_INTERVAL,
	TCA_FQ_CODEL_ECN,
	TCA_FQ_CODEL_FLOWS,
	TCA_FQ_CODEL_QUANTUM,
	TCA_FQ_CODEL_CE_THRESHOLD,
	TCA_FQ_CODEL_DROP_BATCH_SIZE,
	TCA_FQ_CODEL_MEMORY_LIMIT,
	__TCA_FQ_CODEL_MAX
};

#define TCA_FQ_CODEL_MAX (__TCA_FQ_CODEL_MAX - 1)

enum {
	TCA_HHF_UNSPEC,
	TCA_HHF_BACKLOG_LIMIT,
	TCA_HHF_QUANTUM,
	TCA_HHF_HH_FLOWS_LIMIT,
	TCA_HHF_RESET_TIMEOUT,
	TCA_HHF_ADMIT_BYTES,
	TCA_HHF_EVICT_TIMEOUT,
	TCA_HHF_NON_HH_WEIGHT,
	__TCA_HHF_MAX
};

#define TCA_HHF_MAX (__TCA_HHF_MAX - 1)

#define TCQ_ETS_MAX_BANDS 16

enum {
	TCA_ETS_UNSPEC,
	TCA_ETS_NBANDS,
	TCA_ETS_NSTRICT,
	TCA_ETS_QUANTA,
	TCA_ETS_QUANTA_BAND,
	TCA_ETS_PRIOMAP,
	TCA_ETS_PRIOMAP_BAND,
	__TCA_ETS_MAX,
};

#define TCA_ETS_MAX (__TCA_ETS_MAX - 1)

#ifdef __cplusplus
}
#endif

#endif
