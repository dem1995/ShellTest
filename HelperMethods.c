#include <stdbool.h>
#include <stddef.h>


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