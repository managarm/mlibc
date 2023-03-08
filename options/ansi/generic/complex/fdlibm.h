#ifndef __MLIBC_FDLIBM_H
#define __MLIBC_FDLIBM_H

#define REAL_PART(z)    ((z).parts[0])
#define IMAG_PART(z)    ((z).parts[1])

typedef union {
        float complex z;
        float parts[2];
} float_complex;

typedef union {
        double complex z;
        double parts[2];
} double_complex;

#endif
