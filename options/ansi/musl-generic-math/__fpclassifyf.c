#include <math.h>
#include <stdint.h>

int __fpclassifyf(float x)
{
    uint32_t m = (((uint32_t)-1) << 9) >> 9; /* mantissa mask */
    union {float f; uint32_t i;} u = {x};
    /* 0xff + 1 = 0, NaN(1) or Infinity(0)
     * 0x00 + 1 = 1, Subormal(1) or Zero(0)
     * everything else = 2 */
    uint8_t c = (u.i>>23 & 0xff) + 1; /* exponent + 1 */
    c = (c > 2) ? 2 : c;
    c |= ((u.i & m) != 0) << 2; /* m i i */
    return 1 << ((0x22312240 >> (c * 4)) & 0xf);
}
