#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */
#include <stdio.h>     /* printf */
#include <unistd.h>    /* _exit, fork, execvp */
#include "myutils.c"
#include "myutils.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



/************************* Definiciones Globales ********************************/
#define txtSize 80

typedef struct cmd_struct {//Estructura en la que guardamos los comandos y los argumentos de sus comandos leídos del fichero
    char cmd [txtSize];
    char args[10][80];
    int nargs;
  } cmd_type;

/********************************************************************************/


/******************************* Funciones **************************************/

int myWait(){
  int status, res;
  res = wait(&status);
    if ( WIFEXITED(status) ) {   
      const int es = WEXITSTATUS(status);
      if(es == 1) printf("Ha habido un problema al ejecutar el comando\n");
      else if (es == 2) printf("Ha habido un problema al llamar a la función executeCmd");
    }
  return res;
}

int saveCmds(int file, cmd_type *cmd){
  
  char buff[txtSize];
  char ch_ends;
  char old_ch_ends = '\0';
  int ncmds = 0, nargs;

  while(read_split(file, buff, txtSize, &ch_ends) > 0) {
    
    //Esto ocurre cuando leemos el primer comando (primera palabra de la primera línea) que puede tener argumentos o no, por eso el OR
    if((old_ch_ends == '\0' && ch_ends == ' ') || (old_ch_ends == '\0' && ch_ends == '\n')) { 
      
      memcpy(cmd[ncmds].cmd, buff, txtSize);
      ncmds++;
      nargs = 0;

    }

    //Esto ocurre cuando leemos un argumento ya que lo último leído no era un salto de línea sino un espacio
    if((old_ch_ends == ' ' && ch_ends == '\n') || (old_ch_ends == ' ' && ch_ends == ' ') || old_ch_ends == ' '){ 
      
      memcpy(cmd[ncmds-1].args[nargs], buff, txtSize);
      nargs++;
      cmd[ncmds-1].nargs = nargs;

    }

    if(old_ch_ends == '\n'){ //Nuevo comando ya que lo último leído era un salto de línea
      
      /*printf("\n%d) cmd: %s", ncmds, cmd[ncmds-1].cmd);
      if (cmd[ncmds-1].nargs > 0){
        for(int i = 0; i <= cmd[ncmds].nargs; i++){
          printf(" argv[%d]: %s", i+1, cmd[ncmds-1].args[i]);
        }
      }*/
      
      memcpy(cmd[ncmds].cmd, buff, txtSize);
      ncmds++;
      nargs = 0;      
    }

    memset(buff, 0, sizeof buff); //Limpiamos el buffer
    old_ch_ends = ch_ends;
    
  }

  return ncmds;//Devolvemos el número de comandos que hemos guardado en memoria 
  
}

//Función que imprime todos los comandos guardados en memoria junto con sus respectivos argumentos
//Recibe una estructura del tipo cmd_type que contiene toda la info de cada comando, y un entero que indica el número de comandos almacenados en la estructura.


void printSavedCmds(cmd_type *cmd, int ncmds){

  for(int i = 0; i < ncmds; i++){
    
    printf("\n%d) cmd: %s", i+1, cmd[i].cmd);
    if (cmd[i].nargs > 0){      
      for(int j = 0; j < cmd[i].nargs; j++){
        if (!((j == cmd[i].nargs-1) && (strchr("&", *cmd[i].args[j]) != NULL))){ //Si no nos encontramos en el último argumento y el argumento no es un & entonces imprimimos por pantalla
          printf(" argv[%d]: %s", j+1, cmd[i].args[j]);
        }   
      }
    }
  }
}
//Función que imprime solamente el comando que le indiquemos mediante un entero.
void printSomeCmd(cmd_type *cmd, int nCmd){
  
  printf("%s", cmd[nCmd-1].cmd);
  
  if (cmd[nCmd-1].nargs > 0){
    for(int j = 0; j+1 <= cmd[nCmd-1].nargs; j++){
      if (!((j == cmd[nCmd-1].nargs - 1) && strstr(cmd[nCmd-1].args[j], "&") != NULL)){ //Si no nos encontramos en el último argumento y el argumento no es un & entonces imprimimos por pantalla
        printf(" %s", cmd[nCmd-1].args[j]);
      }
    }
  }
}

//Función que ejecutará un comando por terminal. Creara un proceso hijo y el será el que ejecute el comando


void executeCmd(cmd_type *cmd, int nCmd){
  
  char *argv[10];
  
  pid_t pid;
  pid = fork();
       
  //memcpy(argv[0], cmd[nCmd].cmd, txtSize);    
  argv[0] = cmd[nCmd].cmd;
  
  if (cmd[nCmd].nargs > 0){
    if(strchr("&", *cmd[nCmd].args[cmd[nCmd].nargs-1]) != NULL){
      //Hacer ejecucion concurrida
      for (int i = 0; i < cmd[nCmd].nargs-1; i++){
        argv[i+1] = cmd[nCmd].args[i];
      }
      argv[cmd[nCmd].nargs + 1] = NULL;//Para indicar donde acaba la parte útil del array
      //Falta la movida de que sea concurrido
      if(pid == 0){   
        execvp(argv[0], &argv[0]);
        exit(3);
      }
    }else{
      //Hacer ejecución normal (no concurrida)
      for (int i = 0; i < cmd[nCmd].nargs; i++){
        argv[i+1] = cmd[nCmd].args[i];
      }
      argv[cmd[nCmd].nargs + 1] = NULL;//Para indicar donde acaba la parte útil del array
      if(pid == 0){   
        execvp(argv[0], &argv[0]);
      }      
      myWait();//wait con deteccion de errores (hace un print en caso de error dando algo de info)
    }      
  }else{
    argv[cmd[nCmd].nargs + 1] = NULL;//Para indicar donde acaba la parte útil del array
    if(pid == 0){
      execvp(argv[0], &argv[0]);
      _exit(1);
    }    
    myWait();
  }     
  
  printf("\n");
  
  if(pid == 0){//Por si nos falla el execvp de una llamada concurrida aquí llegará un proceso hijo
    _exit(1);//Error de ejecución del comando
  }  
  
}

int main(int argc, char* argv[]){
  
  //printf("Ejercicio 4.4: \n");

  int file = open(argv[1], O_RDONLY, 0644);
  int ncmds, num, exit = 0;
  char buff[txtSize], ch_end;
  
  cmd_type cmds[txtSize];
  
  ncmds = saveCmds(file, cmds);

  while (!exit){
    printf("\n\nMyShell> ");
    fflush( stdout );//para que escriba antes el MyShell>
    read_split(0, buff, txtSize, &ch_end);

    sscanf(buff, "%d", &num);

    if (!strcmp(buff, "list")){
      printSavedCmds(cmds, ncmds);
    }else if(!strcmp(buff, "exit")){
      exit = 1; 
    }else if(num <= 0 || num > ncmds){
      printf("Input inválido, prueba otra vez\n");
    }else{
      //printSomeCmd(cmds, num);
      executeCmd(cmds, num-1);      
    }
     
  }

  while(myWait() > 0); 
  
  
  //printSavedCmds(cmds, ncmds);  

  close(file);

}

/*********************************************************************************/