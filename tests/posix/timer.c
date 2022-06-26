#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

int main() {
	struct timeval a = {0, 0};
	struct timeval b = {0, 0};
	struct timeval res = {0, 0};
	a.tv_sec = 10;
	assert(timerisset(&a) == 1);
	timerclear(&a);
	assert(timerisset(&a) == 0);
	a.tv_sec = 40;
	a.tv_usec = 500;
	b.tv_sec = 10;
	b.tv_usec = 20;
	timeradd(&a, &b, &res);
	assert(res.tv_sec == 50);
	assert(res.tv_usec == 520);
	timerclear(&res);
	timersub(&a, &b, &res);
	assert(res.tv_sec == 30);
	assert(res.tv_usec == 480);
	return 0;
}
