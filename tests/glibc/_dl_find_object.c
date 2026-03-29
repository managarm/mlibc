#include <assert.h>
#include <dlfcn.h>
#include <features.h>
#include <stdio.h>

int main(void) {
	struct dl_find_object dlfo;

	void *addr = &main;
	int ret = _dl_find_object((void *)addr, &dlfo);
	assert(ret == 0);
	assert(addr >= dlfo.dlfo_map_start);
	assert(addr < dlfo.dlfo_map_end);

	addr = &printf;
	ret = _dl_find_object((void *)addr, &dlfo);
	assert(ret == 0);
	assert(addr >= dlfo.dlfo_map_start);
	assert(addr < dlfo.dlfo_map_end);

	ret = _dl_find_object(NULL, &dlfo);
	assert(ret == -1);

	return 0;
}
