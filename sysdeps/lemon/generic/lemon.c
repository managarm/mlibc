#include <lemon/syscall.h>
#include <lemon/lemon.h>

char* lemon_uname(char* string){
	syscall(SYS_UNAME, string, 0, 0, 0, 0);
	return string;
}
