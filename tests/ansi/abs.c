#include <stdlib.h>
#include <assert.h>
#include <limits.h>

int main(){
    assert(abs(-10) == 10);
    assert(abs(2021) == 2021);

    assert(labs(-256) == 256);
    assert(labs(10034890) == 10034890);

    assert(llabs(-0x2deadbeef) == 0x2deadbeef);
    assert(llabs(49238706947) == 49238706947);

    return 0;
}