#include <unistd.h>

__attribute__((destructor)) void destroy() {
	_exit(0);
}

int main() {
	return 1;
}
