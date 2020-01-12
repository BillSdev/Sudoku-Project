
#ifndef FINALPROJECT_PARSER_H
#define FINALPROJECT_PARSER_H

/* This is the header of the Parser module. 
 * In addition to giving access to the main parsing method,
 * It contains the command struct and CommandType enum
 */

#define MAX_ARGS_NUM 3                     /* maximum number of arguments in a command*/
#define MAX_COMMAND_LENGTH 256             /* maximum length of user command*/
#define COMMAND_NUM 18                     /* number of commands available */
#define MAX_COMMAND_NAME_LENGTH 15         /* maximum length of command name*/

/* Enum for all command types ( *order is important* ) */
enum CommandType {
	SOLVE, EDIT, MARK_ERRORS, PRINT_BOARD, GUESS,    /*0 ~ 4*/
	GENERATE, UNDO, REDO, SET, HINT,                 /*5 ~ 9*/
	VALIDATE, RESET, AUTOFILL, SAVE, GUESS_HINT,     /*10 ~ 14*/
	NUM_SOLUTIONS, EXIT, INVALID                     /*15 ~ 17*/
};

/* Enum for argument type ( *order is important* ) */
enum ArgType { X = 0, Y = 1, Z = 2 };

/* Structure representing a command */
typedef struct {
	enum CommandType commandType;
	char commandName[20];
	int arguments[MAX_ARGS_NUM];
	char filePath[MAX_COMMAND_LENGTH];
	float threshold;                  /*maybe think of a better solution later.*/
	int gameStateAvailability[3];     /* 1-yes, 0-no,  indexes are : INIT - 0, SOLVE - 1, EDIT - 2*/
	int argnum;
	int argnumReceived;
	int firstNotNumber;               /*first parameter that is not a number.*/
} Command;

/*
 * Parse a string representing a command into a command structure passed as an argument
 * @param command - pointer to a Command struct we want to update
 * @param cmdstr  - input string to parse
 */
void parseInput(Command *command, char *cmdstr);


#endif
