#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifndef pid_t
#define pid_t int
#endif

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
}


void forkAndLaunch(char** args, FILE* inputFP, FILE* outputFP)
{
	int status;
	pid_t pid;
	switch (pid = fork())
	{
	case -1:
		//syserr("fork");
	case 0:
		if (inputFP != NULL)
			stdin = inputFP;
		if (outputFP != NULL)
			stdout = outputFP;
		execvp(args[0], args);
		//syserr("exec");
	default:
		do
		{
			int w = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}