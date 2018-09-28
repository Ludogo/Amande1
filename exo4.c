#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/* ÉNNONCÉ : réécrire la fonction 'system' -> lance 'bash -c'*/


/* The  system()  library  function uses fork(2) to create a child process
       that executes the shell command specified in command using execl(3)  as
       follows:

           execl("/bin/sh", "sh", "-c", command, (char *) 0);

       system() returns after the command has been completed.

       During  execution  of  the command, SIGCHLD will be blocked, and SIGINT
       and SIGQUIT will be ignored, in the process that calls system()  (these
       signals  will  be  handled according to their defaults inside the child
       process that executes command).

       If command is NULL, then system() returns a status indicating whether a
       shell is available on the system*/

void exit_if(int cond, const char* prefix)
{
  if(!cond) return;
  perror(prefix);
  exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
  execl("/bin/sh", "sh", argv[1], (char *) 0, (char *) 0); //bon ça marche mais c'est interminable -> manipulation de signaux?
  perror("execve");
  exit(EXIT_FAILURE);
}
