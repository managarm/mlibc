#include <assert.h>
#include <wchar.h>
#include <stdlib.h>

int main(void){
    wchar_t *ws1 = calloc(10, sizeof(wchar_t));
    wchar_t *ws2 = calloc(10, sizeof(wchar_t));

    mbstowcs(ws1, "Test 1", 10);
    mbstowcs(ws2, "Tester 2", 10);

    assert(wmemcmp(ws1, ws2, 10) < 0);
    assert(wmemcmp(ws2, ws1, 10) > 0);
    assert(wmemcmp(ws2, ws2, 10) == 0);

	free(ws1);
	free(ws2);
    return 0;
}
