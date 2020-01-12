#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"

/* This module handles parsing the user input into a command struct which we can later use. */

/*constants for game modes used later as indexes*/
#define INIT_STATE 0
#define SOLVE_STATE 1
#define EDIT_STATE 2


/*order is important and based on the command type enum values.
commandNames holds the names of all the commands.*/
static const char *commandNames[COMMAND_NUM] = {
	"solve",         "edit",   "mark_errors",  "print_board",  "guess",
	"generate",      "undo",   "redo",         "set",          "hint",
	"validate",      "reset",  "autofill",     "save",         "guess_hint",
	"num_solutions", "exit",   "invalid"
};
/*commandArgs holds the number of arguments required for each command*/
static const int commandArgs[COMMAND_NUM] = {
	1, 1, 1, 0, 1,
	2, 0, 0, 3, 2,
	0, 0, 0, 1, 2,
	0, 0, 0
};
/*gameStateOption holds the states each command can be executed in.*/
static const int gameStateOption[COMMAND_NUM][3] = {               /*index : 0 - INIT, 1 - SOLVE, 2 - EDIT*/
	{1, 1, 1}, {1, 1, 1}, {0, 1, 0}, {0, 1, 1}, {0, 1, 0},
	{0, 0, 1}, {0, 1, 1}, {0, 1, 1}, {0, 1, 1}, {0, 1, 0},
	{0, 1, 1}, {0, 1, 1}, {0, 1, 0}, {0, 1, 1}, {0, 1, 0},
	{0, 1, 1}, {1, 1, 1}, {1, 1, 1}
};

/* resets the command as we use the same memory throughout the program.
 * sets the default values.
 * set type to invalid so if we couldnt determine any command, it will remain invalid as it should.
 */
void resetCommand(Command *cmd) {
	int i = 0;
	cmd->argnum = 0;
	cmd->argnumReceived = 0;
	cmd->commandType = INVALID;
	cmd->firstNotNumber = -1;
	cmd->filePath[0] = '\0';
	for (i = 0; i < MAX_ARGS_NUM; i++) {
		cmd->arguments[i] = -1;
	}
}

/* Set the type of the command, the name, the number of arguments needed for it and in which modes it is available.*/
void determineCommandTypeAndNumOfArgs(char *cType, Command *command) {
	int i;
	for (i = 0; i < COMMAND_NUM; i++) {
		if (strcmp(cType, commandNames[i]) == 0) {
			command->commandType = i;   /*value of corresponding enum*/
			command->argnum = commandArgs[i];
			strcpy(command->commandName, commandNames[i]);
			command->gameStateAvailability[INIT_STATE] = gameStateOption[i][INIT_STATE];
			command->gameStateAvailability[SOLVE_STATE] = gameStateOption[i][SOLVE_STATE];
			command->gameStateAvailability[EDIT_STATE] = gameStateOption[i][EDIT_STATE];
			return;
		}
	}

}

/* checks if a string is a correct representation of a float.
 * assume that strlen(str) > 0 
 * return 1 if proper representation otherwise return 0.
 */
int isFloat(char *str) {
	int readDot = 0;
	/* we define "i" as unsigned it to avoid warning of comparing
	 * int (i) to unsigned int (strlen(str)) */
	unsigned int i;
	if ((str[0] < 48 || str[0] > 57) && (str[0] != 45))    /* '-' = 45 */
		return 0;
	for (i = 1; i < strlen(str); i++) {
		if ((str[i] < '0' || str[i] > '9') && !(str[i] == '.' && readDot == 0))
			return 0;
		if (str[i] == '.')
			readDot = 1;
	}
	return 1;
}

/* Full documentation in Parser.h */
void parseInput(Command *command, char *cmdstr) {
	char *tokens = strtok(cmdstr, " \t\r\n");
	int i = 0;
	resetCommand(command);
	determineCommandTypeAndNumOfArgs(tokens, command);

	if (command->commandType == INVALID)
		return;

	/*here we are reading the arguments. */
	for (i = 0; i < MAX_ARGS_NUM + 1; i++) {
		tokens = strtok(NULL, " \t\r\n");
		if (tokens == NULL) {
			return;
		}
		/*treat argument as file path if command is edit, solve or save*/
		if (command->commandType == SOLVE || command->commandType == EDIT || command->commandType == SAVE) {
			strcpy(command->filePath, tokens);
		}
		/*treat argument as float for guess command*/
		else if (command->commandType == GUESS) {
			if (isFloat(tokens))
				command->threshold = atof(tokens);
			else if (command->firstNotNumber == -1)
				command->firstNotNumber = i + 1;
		}
		/*otherwise treat as integer*/
		else {
			int val;
			val = atoi(tokens);
			if (strlen(tokens) == 1 && tokens[0] == 48) {  /*atoi() returns 0 for illegal conversion*/
				command->arguments[i] = 0;                     /*so we distinguish between real 0 parameter and atoi() 0.*/
				command->argnumReceived++;
				continue;
			}
			if (val == 0 && command->firstNotNumber == -1)
				command->firstNotNumber = i + 1;
			command->arguments[i] = val;
		}
		command->argnumReceived++;
	}
}





