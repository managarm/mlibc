#include <assert.h>
#include <stdlib.h>
#include <sched.h>

#define SET_SIZE 15

int main() {
	cpu_set_t *set = CPU_ALLOC(SET_SIZE);
	size_t setsize = CPU_ALLOC_SIZE(SET_SIZE);

	CPU_ZERO_S(setsize, set);

	assert(!CPU_ISSET_S(11, setsize, set));

	CPU_SET_S(11, setsize, set);
	assert(CPU_ISSET_S(11, setsize, set));
	assert(CPU_COUNT_S(setsize, set) == 1);

	assert(!CPU_ISSET_S(CPU_SETSIZE - 1, setsize, set));

	CPU_CLR_S(11, setsize, set);
	assert(!CPU_ISSET_S(11, setsize, set));

	CPU_FREE(set);

	return 0;
}
