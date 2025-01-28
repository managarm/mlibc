#ifndef _GSHADOW_H
#define _GSHADOW_H

#include <paths.h>
#include <bits/size_t.h>

#define GSHADOW _PATH_GSHADOW

struct sgrp {
    char *sg_namp;
    char *sg_passwd;
    char **sg_adm;
    char **sg_mem;
};

#ifndef __MLIBC_ABI_ONLY

#ifdef __cplusplus
extern "C" {
#endif

int getsgnam_r(const char *__name, struct sgrp *__result_buf, char *__buffer, size_t __len, struct sgrp **__result);

#ifdef __cplusplus
}
#endif

#endif /* !__MLIBC_ABI_ONLY */

#endif
