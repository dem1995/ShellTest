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



#define MAX_BUFFER 1024 // max line buffer
#define MAX_ARGS 64 // max # args
#define SEPARATORS " \t\n" // token sparators
#define KBLU  "\x1B[34m"	//Blue text
#define KCYN  "\x1B[36m"	//Cyan text
#define KRED  "\x1B[31m"	//Red text
#define RESET "\x1B[0m"		//Reset text color

void bashLaunch(char* command);
bool customCommandCheck(char* arg0, char** args, char* inputFS, char* outputFS);

extern char** environ;
int main(int argc, char ** argv) {
	char buf[MAX_BUFFER]; // line buffer
	char* args[MAX_ARGS]; // pointers to arg strings
	char** arg; // working pointer thru args
	char* prompt = "==>"; // shell prompt
	FILE* inputFP = NULL;
	FILE* outputFP = NULL;

	/* keep reading input until "quit" command or eof of redirected input */
	while (!feof(stdin)) {

		fprintf(stdout, KCYN"%s"RESET"%s ", getenv("PWD"), prompt); //write prompt


		if (fgets(buf, MAX_BUFFER, stdin)) // read a line
		{ 

			/*TOKENIZING THE INPUT*/
			arg = args;
			*arg++ = strtok(buf, SEPARATORS);
			while ((*arg++ = strtok(NULL, SEPARATORS))); // last entry will be NULL	


			if (args[0]) // if there's anything there
			{

				/*HANDLING I/O*/
				char inputString[MAX_BUFFER] = "";
				char outputString[MAX_BUFFER] = "";
				determineRedirection(args, inputString, outputString);
				setUpIO(inputString, outputString, &inputFP, &outputFP);

				/*CHECKING FOR COMMANDS*/
				// check for internal commands
				if (customCommandCheck(args[0], args, inputFP, outputFP))
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
	if (inputFP != NULL)
		fclose(inputFP);
	if (outputFP != NULL)
		fclose(outputFP);

	return 0;
}

bool customCommandCheck(char* arg0, char** args, FILE* inputFP, FILE* outputFP)
{
	/*CLEAR COMMAND*/
	if (!strcmp(args[0], "clr")) //"clear" command
	{
		args[0] = "clear";
		forkAndLaunch(args, inputFP, outputFP);
	}

	/*DIRECTORY COMMAND*/
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

	/*ENVIRON COMMAND*/
	else if (!strcmp(args[0], "environ"))
	{
		//FILE* output = fopen(outputFS, "w");


		char** env = environ;
		if (outputFP==NULL)
			while (*env)
				fprintf(stdout, "%s\n", *env++); // step through environment
		else
			while (*env)
				fprintf(outputFP, "%s\n", *env++); // step through environment
	}

	/*CHANGE DIRECTORY COMMAND*/
	else if (!strcmp(args[0], "cd"))	
	{
		if (args[1] == NULL) //if there's no second argument, just print the current directory
		{ 
			fprintf(stdout, "Current directory (According to environ): %s\n", getenv("PWD"));
		}
		else				//If there is a second argument
		{
			if (chdir(args[1])==0) 
			{
				char* buf = malloc(sizeof(char)*MAX_BUFFER);
				setenv("PWD", getcwd(buf, MAX_BUFFER), 1);
				free(buf);
			}
			else {
				fprintf(stdout, "There was a problem with changing directories.\n");

			}
		}
	}
	else
		return false;
	return true;
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



