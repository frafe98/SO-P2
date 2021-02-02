#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */
#include <stdio.h>     /* printf */
#include <unistd.h>    /* _exit, fork, execvp */
#include "myutils.c"
#include "myutils.h"
#include <string.h>


int main(int argc, char* argv[]){
  const int txtSize = 80;
  printf("Ejercicio 4.2 y 4.3: \n");

  typedef struct cmd_struct {
    char cmd [txtSize];
    char args[10][80];
    int nargs;
  } cmd_type;

  int ncmds = 0, nargs;
  cmd_type cmd[txtSize];
  char buff[txtSize];


  int file2 = open(argv[1], O_RDONLY | O_RDWR, 0644);
  char ch_ends;
  char old_ch_ends = '\0';
  
  while(read_split(file2, buff, txtSize, &ch_ends) > 0) {
    
    //Esto ocurre cuando leemos el primer comando (primera palabra de la primera línea) que puede tener argumentos o no, por eso el OR
    if((old_ch_ends == '\0' && ch_ends == ' ') || (old_ch_ends == '\0' && ch_ends == '\n')) { 
      
      memcpy(cmd[ncmds].cmd, buff, txtSize);
      ncmds++;
      nargs = 0;

    }

    //Esto ocurre cuando leemos un argumento ya que lo último leído no era un salto de línea sino un espacio
    if((old_ch_ends == ' ' && ch_ends == '\n') || (old_ch_ends == ' ' && ch_ends == ' ')){ 
      
      memcpy(cmd[ncmds-1].args[nargs], buff, txtSize);
      nargs++;
      cmd[ncmds-1].nargs = nargs;

    }

    if(old_ch_ends == '\n'){ //Nuevo comando ya que lo último leído era un salto de línea
      
      printf("\n%d) cmd: %s", ncmds, cmd[ncmds-1].cmd);
      if (cmd[ncmds-1].nargs > 0){
        for(int i = 0; i <= cmd[ncmds].nargs; i++){
          printf(" argv[%d]: %s", i+1, cmd[ncmds-1].args[i]);
        }
      }
      
      memcpy(cmd[ncmds].cmd, buff, txtSize);
      ncmds++;
      nargs = 0;      
    }

    old_ch_ends = ch_ends;
  }

  //Para printar el último elemento leído.
  printf("\n%d) cmd: %s", ncmds, cmd[ncmds-1].cmd);
      if (cmd[ncmds-1].nargs > 0){
        for(int i = 0; i <= cmd[ncmds].nargs; i++){
          printf(" argv[%d]: %s", i+1, cmd[ncmds-1].args[i]);
        }
      }

  close(file2);

}
