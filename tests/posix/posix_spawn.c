#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

void run_cmd(char *cmd)
{
    pid_t pid;
    char *argv[] = {"sh", "-c", cmd, NULL};
    int status;
    printf("Run command: %s\n", cmd);
    status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
    if(status == 0) {
        printf("Child pid: %i\n", pid);
        if(waitpid(pid, &status, 0) != -1) {
            printf("Child exited with status %i\n", status);
        printf("Child exit status: %i\n", WEXITSTATUS(status));
        assert(WEXITSTATUS(status) == 0);
        } else {
            perror("waitpid");
        assert(0 == 1);
        }
    } else {
        printf("posix_spawn: %s\n", strerror(status));
    assert(0 == 1);
    }
}

int main() {
    run_cmd(":");
    return 0;
}
