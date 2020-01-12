#ifndef FINALPROJECT_GAME_H
#define FINALPROJECT_GAME_H
#include "IOHandler.h"
#include "Solver.h"

/* Header of the main Game module.  Provides in game functions */

/* enum representing the different types of available modes. */
enum Mode{Solve = 1,Edit = 2,Init = 0} mode;

/*
 * load the file for editing into the board and set all the values of the board to be not positive to
 * mark them as either empty (0) or fixed (negative).
 * @param filePath - file from which we load the board.
 * return 1 if succeessfully loaded the board, otherwise return -1.
 */
int loadEdit(char* filePath);

/*
 * load the file for solving into the board
 * @param filePath - file from which we load the board.
 * return 1 if successfully loaded the board, otherwise return -1.
 */
int loadSolve(char* filePath);

/* 
 * Removes or Shows error markings.
 * @param newMark - value of 1 - mark errors, value of 0 - remove error marks.
 * return 1 on successful execution otherwise return -1.
 */
int mark_errors(int newMark);

/*
 * Set a value for a given cell.
 * @param i - row of the cell ( Y value )
 * @param j - column of the cell ( X value )
 * @param value - value to set
 * return 1 if the value was successfully set, otherwise return -1.
 */
int setCell(int i,int j,int value);

/*
 * Save the current board into a given file. 
 * Create a new file name filePath in the case that it doesn't exist.
 * @param filePath - the name of the file we want to save the board into.
 * return 1 if the board was successfully saved, otherwise return -1.
 */
int save(char* filePath);

/*
 * Print the game board
 * @param boardGame - the board we want to print
 */
void printBoard(int** boardGame);

/*
 * For a given (X, Y) cell,  guess which values can be correct 
 * and provide a score for each of those values. Higher score = higher chance it will be correct.
 * @param x - The X coordinate of the cell ( column )
 * @param y - The Y coordinate of the cell ( row )
 * return 1 if the guessing process was successful, otherwise return -1.
 */
int guess_hint(int x, int y);

/*
 * For every cell in the board, randomly guess a legal value 
 * out of all the possible values that were given a score equal or higher than the given threshold.
 * @param threshold - score threshold for values to be considered in the random value picking decision
 * return 1 if the guessing process was executed successfuly, otherwise return -1.
 */
int guess(float threshold);

/*
 * Check if the current solution is valid and correct.
 * return 1 if the solution is correct, otherwise return -1.
 */
int checkWin();

/*
 * Generates a puzzle by randomly filling X empty cells with legal values,
 * solving the board, and then clearing all but Y random cells.
 * @param x - The X coordinate of the cell ( column )
 * @param y - The Y coordinate of the cell ( row )
 * return 1 if successfully generated the cells, otherwise return -1.
 */
int generate(int x, int y);

/*
 * Initialize the board
 */
int initBoard();

#endif
