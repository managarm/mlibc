#include <assert.h>
#include <stdint.h>

extern _Thread_local char foo_thread_local[];
extern _Thread_local char bar_thread_local[];

int main() {
	assert(!((uintptr_t)foo_thread_local & (16 - 1)));
	assert(!((uintptr_t)bar_thread_local & (8 - 1)));

	return 0;
}
