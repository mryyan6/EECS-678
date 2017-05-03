#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BSIZE 256

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"

int main(int argc, char *argv[])
{
	int status;
	pid_t pid_1, pid_2, pid_3, pid_4;
	int pipefd1[2], pipefd2[2], pipefd3[2];
	
	pipe(pipefd1);
	pipe(pipefd2);
	pipe(pipefd3);
	
	if (argc != 4) {
		printf("usage: finder DIR STR NUM_FILES\n");
		exit(0);
	}
	

 pid_1 = fork();
  if (pid_1 == 0) {
    /* First Child */
    dup2(pipefd1[1],fileno(stdout));
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    sprintf(cmdbuf, "%s %s -name \'*\'.[ch]", FIND_EXEC, argv[1]);
    /* set up pipes */
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    close(pipefd3[0]);
    close(pipefd3[1]);
    
    
    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  pid_2 = fork();
  if (pid_2 == 0) {
    /* Second Child */
	  
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    dup2(pipefd1[0],fileno(stdin));
    dup2(pipefd2[1],fileno(stdout));  
    sprintf(cmdbuf, "%s %s -c %s", XARGS_EXEC, GREP_EXEC, argv[2]);
   
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    close(pipefd3[0]);
    close(pipefd3[1]);
    
    
    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  pid_3 = fork();
  if (pid_3 == 0) {
 	 
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    dup2(pipefd2[0],fileno(stdin));
    dup2(pipefd3[1],fileno(stdout));
    
    sprintf(cmdbuf, "%s -t : +1.0 -2.0 --numeric --reverse", SORT_EXEC);

    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    close(pipefd3[0]);
    close(pipefd3[1]);
    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }
  
  pid_4 = fork();
  if (pid_4 == 0) {
	 
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    dup2(pipefd3[0],fileno(stdin));

    sprintf(cmdbuf, "%s --lines=%s",HEAD_EXEC, argv[3]);
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    close(pipefd3[0]);
    close(pipefd3[1]);
    
    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }
	 close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);
    close(pipefd3[0]);
    close(pipefd3[1]);
 
	if ((waitpid(pid_1, &status, 0)) == -1) {
		fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}
	if ((waitpid(pid_2, &status, 0)) == -1) {
		fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}
	if ((waitpid(pid_3, &status, 0)) ==  -1) {
		fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}
	if ((waitpid(pid_4, &status, 0)) == -1) {
		fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
		return EXIT_FAILURE;
	}

	return 0;
}


