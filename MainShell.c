/*
 strtokeg - skeleton shell using strtok to parse command line

 usage:

 ./a.out

 reads in a line of keyboard input at a time, parsing it into
 tokens that are separated by white spaces (set by #define
 SEPARATORS).
 can use redirected input

 if the first token is a recognized internal command, then that
 command is executed. otherwise the tokens are printed on the
 display.
strtokeg.c
C
C

 internal commands:

 clear - clears the screen

 quit - exits from the program

 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "HelperMethods.c"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef pid_t
#define pid_t int
#endif


void setUpIO(char* inputString, char* outputString);
void bashLaunch(char* command);
void forkAndLaunch(char** args);
bool customCommandCheck(char* arg0, char** args);


#define MAX_BUFFER 1024 // max line buffer
#define MAX_ARGS 64 // max # args
#define SEPARATORS " \t\n" // token sparators

extern char** environ;
int main(int argc, char ** argv) {
	char buf[MAX_BUFFER]; // line buffer
	char* args[MAX_ARGS]; // pointers to arg strings
	char** arg; // working pointer thru args
	char* prompt = "==>"; // shell prompt
	/* keep reading input until "quit" command or eof of redirected input */

	while (!feof(stdin)) {

		/* get command line from input */
		fputs(prompt, stdout); // write prompt
		if (fgets(buf, MAX_BUFFER, stdin)) { // read a line

			/* tokenize the input into args array */
			arg = args;
			*arg++ = strtok(buf, SEPARATORS); // tokenize input
			while ((*arg++ = strtok(NULL, SEPARATORS)));
			// last entry will be NULL
			if (args[0])
			{
				// if there's anything there

				/*HANDLING DIRECTORY STUFF*/

				/*HANDLING I/O*/
				char inputString[MAX_BUFFER] = "";
				char outputString[MAX_BUFFER] = "";
				determineRedirection(args, inputString, outputString);
				//setUpIO(inputString, outputString);

				/*CHECKING FOR COMMANDS*/
				// check for internal/external command
				if (customCommandCheck(args[0], args))
					continue;
				// check for quitting
				else if (!strcmp(args[0], "quit")) // "quit" command
					break; // break out of 'while' look
				//else pass command on to BASH
				else
					bashLaunch(buf);
			}
		}
	}
	return 0;
}


void setUpIO(char* inputString, char* outputString)
{
	if (strcmp(inputString, "")!=0)	//if there's an input string
	{ 
		int fd = open(inputString, O_RDONLY);
		dup2(fd, STDIN_FILENO);
		close(fd);
	}

	if (strcmp(outputString, "")!=0) //if there's an output string
	{
		int fd = open(inputString, O_WRONLY | O_CREAT | O_TRUNC);
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
}

bool customCommandCheck(char* arg0, char** args)
{
	if (!strcmp(args[0], "clr")) //"clear" command
	{
		args[0] = "clear";
		forkAndLaunch(args);
	}
	else if (!strcmp(args[0], "dir"))	//"directory" command
	{
		char* dir = malloc(sizeof(char) * MAX_BUFFER);
		char* dircmdmodifier = malloc(sizeof(char) * MAX_BUFFER); //modifier for how dir should be displayed
		if (args[1] != 0)
		{
			// if the directory parameter is non-empty
			strcpy(dircmdmodifier, "ls -al ");
			strcpy(dir, args[1]);
			char* cmd = strcat(dircmdmodifier, dir);
			bashLaunch(cmd);
		}
		else
		{
			// if no directory is specified, use the current directory.
			bashLaunch("ls -al ./");
		}
		free(dir);
		free(dircmdmodifier);
	}
	else if (!strcmp(args[0], "environ"))	//"environ" command
	{
		char** env = environ;
		while (*env)
			printf("%s\n", *env++); // step through environment
	}
	else if (!strcmp(args[0], "cd"))	//"change directory" command
	{
		if (args[1] == NULL) //if there's no second argument, just print the current directory
		{ 
			printf("Current directory (According to environ): %s\n", getenv("PWD"));
		}
		else				//If there is a second argument
		{
			if (chdir(args[1])==0) 
			{
				char* buf = malloc(sizeof(char)*MAX_BUFFER);
				setenv("PWD", getcwd(NULL, MAX_BUFFER), 1);
				free(buf);
			}
			else {
				printf("There was a problem with changing directories.\n");
			}
		}
	}
	else
		return false;
	return true;
}



void forkAndLaunch(char** args)
{
	int status;
	pid_t pid;
	switch (pid = fork())
	{
		case -1:
			//syserr("fork");
		case 0:
			execvp(args[0], args);
			//syserr("exec");
		default:
			do 
			{
				int w = waitpid(pid, &status, WUNTRACED);
			}	while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

void bashLaunch(char* command)
{
	int status;
	pid_t pid = fork();
	if (pid == 0)
	{
		execl("/bin/bash", "sh", "-c", command, (char *)0);
	}
	else
	{
		do
		{
			int w = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}
