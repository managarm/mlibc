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

int getsgnam_r(const char *name, struct sgrp *result_buf, char *buffer, size_t len, struct sgrp **result);

#endif /* !__MLIBC_ABI_ONLY */

#endif
