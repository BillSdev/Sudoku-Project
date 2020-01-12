#include "MainAux.h"
#include <time.h>

static char* input;
static Command* command;


int main() {
    int skip;

	srand(time(NULL));

	input = (char*)malloc(sizeof(char)*(MAX_COMMAND_LENGTH + 2));
	command = (Command*)malloc(sizeof(Command));
	init();
	/*reading input until user exists or the input ends.*/
	while (isRunning() && feof(stdin) != 1 && fgets(input, MAX_COMMAND_LENGTH + 2, stdin) != NULL) {
		if (strlen(input) == MAX_COMMAND_LENGTH + 1 && input[MAX_COMMAND_LENGTH] != '\n') {
			printf("The command is too long. Please make sure it does not pass %d characters.\n", MAX_COMMAND_LENGTH);
			while (getchar() != '\n') {}
			continue;
		}
		 skip = 0;  /* avoid changing input pointer as it points to the beginning of the allocated block. */
		while (*(input + skip) == ' ' || *(input + skip) == '\t' || *(input + skip) == '\r' || *(input + skip) == '\n') {
			skip++;
		}
		if (strlen((input + skip)) == 0)
			continue;
		parseInput(command, (input + skip));

		if (checkCommandValidity(command) == 0)
			continue;

		if (runCommand(command) == 1) {
			enum CommandType ct = command->commandType;
			/*printf("\nSuccessfuly executed the command. Delete later. (in main.c)");*/
			if (ct == EDIT || ct == SOLVE || ct == SET || ct == AUTOFILL || ct == UNDO
			 || ct == REDO || ct == GENERATE || ct == GUESS ||  ct == RESET)
				printGame();
			if (ct == GUESS || ct == SOLVE || ct == AUTOFILL || ct == SET) 
				checkWin();
			
		}
		printf("\n");

	}

	/*Free everything before exiting the program*/
	freeAll();
	if (command != NULL) {
		free(command);
		command = NULL;
	}
	if (input != NULL) {
		free(input);
		input = NULL;
	}

	return 0;

}

