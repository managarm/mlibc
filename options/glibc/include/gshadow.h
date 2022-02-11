#ifndef _GSHADOW_H
#define _GSHADOW_H

#include <paths.h>

#define GSHADOW _PATH_GSHADOW

struct sgrp {
    char *sg_namp;
    char *sg_passwd;
    char **sg_adm;
    char **sg_mem;
};

#endif
