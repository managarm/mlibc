#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
	int res;

	res = system("true");
	assert(WIFEXITED(res));
	assert(WEXITSTATUS(res) == 0);

	res = system("false");
	assert(WIFEXITED(res));
	assert(WEXITSTATUS(res) == 1);

	res = system(NULL);
	assert(res == 1);

	return 0;
}
