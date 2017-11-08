#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef pid_t
#define pid_t int
#endif

void removeRedirectionElements(char** args);
void removeString(char** stringArray, int location);

void determineRedirection(char** argStrings, char* inputString, char* outputString)
{
	bool nextStringIsInputToken = false;
	bool nextStringIsOutputToken = false;

	for (int i = 0; argStrings[i] != NULL; i++)
	{

		if (nextStringIsInputToken)
		{
			strcpy(inputString, argStrings[i]);
			nextStringIsInputToken = false;
		}

		else if (nextStringIsOutputToken)
		{
			strcpy(outputString, argStrings[i]);
			nextStringIsOutputToken = false;
		}

		if (strcmp(argStrings[i], "<") == 0)
		{
			nextStringIsInputToken = true;
		}
		else if (strcmp(argStrings[i], ">") == 0)
		{
			nextStringIsOutputToken = true;
		}
	}

	removeRedirectionElements(argStrings);
}


//Clears *inputFPPointer and *outputFPPointer if they're not NULL
void setUpIO(char* inputString, char* outputString, FILE** inputFPPointer, FILE** outputFPPointer)
{
	if (*inputFPPointer != NULL)
	{
		fclose(*inputFPPointer);
		*inputFPPointer = NULL;
	}
	if (*outputFPPointer != NULL)
	{
		fclose(*outputFPPointer);
		*outputFPPointer = NULL;
	}

	if (strcmp(inputString, "") != 0)	//if there's an input string
	{
		*inputFPPointer = fopen(inputString, "r");
		//freopen(inputString, "r", stdin);
		//stdin = fopen(inputString, "r");
		//int fd = open(inputString, O_RDONLY);
		//dup2(fd, STDIN_FILENO);
		//close(fd);
	}
	if (strcmp(outputString, "") != 0) //if there's an output string
	{
		*outputFPPointer = fopen(outputString, "w");
		//freopen(outputString, "w", stdout);
		//int fd = open(inputString, O_WRONLY | O_CREAT | O_TRUNC);
		//dup2(fd, STDOUT_FILENO);
		//close(fd);
	}
}

void forkAndLaunch(char** args, char* inputFS, char* outputFS)
{
	int status;
	pid_t pid;
	switch (pid = fork())
	{
	case -1:
		//syserr("fork");
	case 0:
		if (!strcmp(inputFS, "")==0)
		//if (inputFP!=NULL)
		{
			//dup2(fileno(inputFP), STDIN_FILENO);
			//fclose(inputFP);
			//stdin = inputFP;
			freopen(inputFS, "r", stdin);
		}
		if (!strcmp(outputFS, "") == 0)
		//if(outputFP!=NULL)
		{
			//dup2(fileno(inputFP), STDOUT_FILENO);
			//fclose(outputFP);
			//stdout = outputFP;
			freopen(outputFS, "w", stdout);
		}
		execvp(args[0], args);
		//syserr("exec");
	default:
		do
		{
			int w = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

void removeRedirectionElements(char** args)
{
	for (int i=0; args[i] != NULL; i++)
	{
		if ((strcmp(args[i], "<") == 0) || (strcmp(args[i], ">") == 0) || (strcmp(args[i], ">>") == 0))
		{
			removeString(args, i);
			if (args[i]!=NULL)
				removeString(args, i);
		}
	}
}

void removeString(char** stringArray, int location)
{
	for ( int i=location; stringArray[i] != NULL; i++)
	{
		stringArray[i] = stringArray[i + 1];
	}
}

