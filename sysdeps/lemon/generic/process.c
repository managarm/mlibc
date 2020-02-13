#include <lemon/syscall.h>

int lemon_spawn(char* path){
	syscall(SYS_EXEC, path, 0, 0, 0, 0);
}

int lemon_spawnv(char* path, const char* argv[]){

}

int lemon_kill(int pid){

}
