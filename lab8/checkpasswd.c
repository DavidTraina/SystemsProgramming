#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      	perror("fgets");
      	exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      	perror("fgets");
      	exit(1);
  }
  int fd[2];
  if (pipe(fd) == -1) {
  		perror("pipe");
  		exit(1);
  }
  if (write(fd[1], user_id, MAX_PASSWORD) < 0) {
  		perror("write");
  		exit(1);
  }
  if (write(fd[1], password, MAX_PASSWORD) < 0) { // Why need MAX_PASSWORD bytes??? just always read and write same number of bytes
  		perror("write");
  		exit(1);
  }
  if (close(fd[1])) {// done writing
  		perror("close");
  		exit(1);
  } 
  int n = fork();
  if (n < 0) {
  		perror("fork");
        exit(1);
  }
  if (dup2(fd[0], STDIN_FILENO) == -1) { // after fork so it happens in child too
  		perror("dup2");
  		exit(1);
  } 

  if (n == 0) {
  		if (-1 == execl("./validate", "validate", (char *) NULL)) {
  			perror("execl");
  			exit(1);
  		}
  }
  // Will only execute in parent b/c exec takes over child process and will not continue here
  if (close(fd[0])) { // ***THIS MUST HAPPEN AFTER dup2***
  	  	perror("close");
  		exit(1);
  } 
  int status;
  wait(&status);
  if (WIFEXITED(status) != 0) { // Terminated normally
  		switch(WEXITSTATUS(status)) {
  			case 0:
  				printf(SUCCESS);
  				break;
  			case 2:
  				printf(INVALID);
  				break;
  			case 3:
  				printf(NO_USER);
  				break;
  		}

  }
  return 0;
}
