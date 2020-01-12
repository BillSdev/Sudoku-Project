#ifndef FINALPROJECT_MAINAUX_H
#define FINALPROJECT_MAINAUX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Game.h"
#include "Parser.h"

/*
 * Print the whole game
 */
void printGame();

/*
 * initiate the game
 */
void init();

/*
 * Check whether the game is still running or not
 * return 1 if running, otherwise return 0.
 */
int isRunning();

/*
 * load file in edit mode
 * @param fileName - the path and name of the file to be loaded
 * return 1 if successfully loaded into edit mode with the given puzzle, otherwise return -1.
 */
int edit(char* fileName);

/*
 * Initialize new game in edit mode and default 9x9 empty board
 * return 1 if successfully loaded into edit mode with default board, otherwise return -1.
 */
int editDefault();

/*
 * load file in solve mode
 * @param fileName - the path and name of the file to be loaded
 * return 1 if successfully loaded into solve mode with the given puzzle, otherwise return -1.
 */
int solve(char* fileName);

/* Execute a given command. 
 * @param command - a command struct that represents the given command.
 * returns 1 if the command was successfully executed, otherwise return -1.
 */
int runCommand(Command* command);

/*
 * Check if the command valid.
 * A valid command in this case is an existing command
 * called in a supported mode with a corrent number of parameters.
 * @param command - a command struct that represents the given command.
 * return 1 if the command is valid (by the above definition) otherwise return -1.
 */
int checkCommandValidity(Command *command);

/* 
 * Free game resources.
 */
void freeAll();

#endif
