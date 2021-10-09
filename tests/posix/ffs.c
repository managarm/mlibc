#include <assert.h>
#include <strings.h>

int main(void){
    assert(ffs(0x8000) == 16);
    assert(ffs(0x0) == 0);
    assert(ffs(0x8000000000000000) == 0);

    return 0;
}
