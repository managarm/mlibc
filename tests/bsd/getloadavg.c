#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    double samples[3];
    assert(getloadavg(samples, 3) != 0);
    printf("%f %f %f\n", samples[0], samples[1], samples[2]);
    return 0;
}
