#include <stdio.h>
#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */
#include <stdio.h>     /* printf */
#include <unistd.h>    /* _exit, fork, execvp */

int main(int argc, char* argv[]) {
    pid_t pid;
    pid = fork();
    printf("Father: my pid is %d\n", pid);

    if (pid == 0) {  
        printf("Child process: My pid is %d\n", pid);
        if(argc > 1) {
           printf("-------------- Child doing exec: %s\n", argv[1]);
           execvp(argv[1], &argv[1]);
            _exit(2);
        } 
        _exit(1);
    } 
    printf("Father: Gonna wait for Child\n");

  return 0;
}