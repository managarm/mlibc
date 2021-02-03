// Code modified from https://cplusplus.com
#include <stdlib.h>
#include <assert.h>

int main () {
	char szNumbers[] = "2001 60c0c0 -1101110100110100100000 0x6fffff +2001";
	char *pEnd;
	long int li1, li2, li3, li4, li5;
	li1 = strtol(szNumbers, &pEnd, 10);
	li2 = strtol(pEnd, &pEnd, 16);
	li3 = strtol(pEnd, &pEnd, 2);
	li4 = strtol(pEnd, &pEnd, 0);
	li5 = strtol(pEnd, NULL, 10);
	assert(li1 == 2001);
	assert(li2 == 6340800);
	assert(li3 == -3624224);
	assert(li4 == 7340031);
	assert(li5 == 2001);
	return 0;
}
