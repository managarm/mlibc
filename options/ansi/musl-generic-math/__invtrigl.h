/* shared by acosl, asinl and atan2l */
#define pio2_hi __pio2_hi
#define pio2_lo __pio2_lo

#ifndef __MLIBC_ABI_ONLY

extern const long double pio2_hi, pio2_lo;

long double __invtrigl_R(long double z);

#endif /* !__MLIBC_ABI_ONLY */
