#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */
#include <stdio.h>     /* printf */
#include <unistd.h>    /* _exit, fork, execvp */
#include "myutils.c"
#include "myutils.h"
#include <string.h>

int main(int argc, char* argv[]){
  //EJERCICIO 1
  
  printf("Ejercicio 4.1: \n");
  int file = open(argv[1], O_RDONLY | O_RDWR, 0644);
  char cmds[100];
  char ch_end;
  int i = 1;

  while(read_split(file ,cmds, 100, &ch_end) > 0){
      printf("%d) ", i);
      printf("%s\n", cmds);
      i++;
  } 
  close(file);
}