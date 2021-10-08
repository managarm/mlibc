#include <assert.h>
#include <string.h>

int main(void){
    long test1 = 1L << (sizeof(long) * 8 - 1);
    long long test2 = 1LL << (sizeof(long long) * 8 - 1);

    assert(ffsl(test1) == sizeof(long) * 8);
    assert(ffsll(test2) == sizeof(long long) * 8);
    assert(ffsl(0) == 0);
    assert(ffsll(0) == 0);
    assert(ffsl(test1) == ffsll(test1));
    if(sizeof(long) < sizeof(long long)){
        assert(ffsl(test1) < ffsll(test2));
    } else {
        assert(ffsl(test2) == ffsll(test2));
    }

    return 0;
}
