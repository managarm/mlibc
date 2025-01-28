#ifndef _ABIBITS_SHM_H
#define _ABIBITS_SHM_H

#ifdef __cplusplus
extern "C" {
#endif

struct shm_info {
	int used_ids;
	unsigned long shm_tot;
	unsigned long shm_rss;
	unsigned long shm_swp;
	unsigned long swap_attempts;
	unsigned long swap_successes;
};

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SHM_H */
