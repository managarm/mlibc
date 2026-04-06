#ifndef _ABIBITS_SIGVAL_H
#define _ABIBITS_SIGVAL_H

#ifdef __cplusplus
extern "C" {
#endif

union sigval {
	int sival_int;
	void *sival_ptr;
};

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGVAL_H */
