#include <stdio.h>
#include <assert.h>

int mainDone = 0;

int isFooDone();

void preInit1() {
	// Use dprintf because stdout might not be initialized yet.
	dprintf(1, "pre-initialization function 1 called in main executable\n");

	assert(isFooDone() == 0);
	assert(mainDone == 0);
	mainDone++;
}

void preInit2() {
	dprintf(1, "pre-initialization function 2 called in main executable\n");

	assert(isFooDone() == 0);
	assert(mainDone == 1);
	mainDone++;
}

__attribute__((constructor))
void mainInit() {
	dprintf(1, "initialization function called in main executable\n");

	assert(isFooDone() == 1);
	assert(mainDone == 2);
	mainDone++;
}

// Manually register the pre-initialization functions.
__attribute__((used, section(".preinit_array")))
static void (*preinitFunctions[])(void) = {
	&preInit1,
	&preInit2,
};

int main() {
	assert(isFooDone() == 1);
	assert(mainDone == 3);

	return 0;
}
