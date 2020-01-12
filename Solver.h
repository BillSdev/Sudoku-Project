#ifndef FINALPROJECT_SOLVER_H
#define FINALPROJECT_SOLVER_H
#include "List.h"
#include "Stack.h"
#include "GurobiHelper.h"

extern int boardSize;
extern int blockWidth;
extern int blockHeight;

/*
 * Check if the board is erroneous
 * @param board - Board to check
 * return 1 if valid, otherwise return -1
 */
int validateCurrentStatus(int** board);

/*
 * Given a cell we check if there is cell in the same row / column / block
 * with the same value.
 * @param board - current board to check.
 * @param i,j - the cell indexes.
 * return 1 if there is no same value in cells in the same row \ column \ block
 * otherwise return -1
 */
int validateCell(int** board,int i,int j);

/*
 * Automatically fill "obvious" values – cells which contain a single legal value
 * @param board - The game board
 * return 1 if autofill was executed successfully, otherwise return -1
 */
int autofill(int** board);

/*
 * return the number of possible solutions for the current board
 * @param board - The game board
 * return num of solutions. If an error occured, return -1
 */
int num_solutions(int** board);

/*
 * Check whether the board has a valid solution
 * @param board - The game board
 * return 1 if the board has a valid solution, return 0 if board is unsolvable
 * return -1 if there was an error determining validity.
 */
int validate(int** board);

/*
 * Gives a hint about a possbile value for cell (x, y).
 * @param x , y  -  cell coordinates
 * returns the result if successfully gave a hint
 * otherwise, if an error occured return -1
 */
int hint(int x,int y,int** board);

/*
 * If scores argument is not NULL, for cell (r, c) we guess which values can be correct 
 * and provide a score for each of those values and insert the scores into the scores array argument.
 * If scores argument is NULL, for every cell in the board, randomly guess a legal value 
 * out of all the possible values that were given a score equal or higher than the given threshold.
 * @param board - The game board
 * @param threshold - Score threshold for values to have a chance to be chosen
 * @param r, c - Cell indexes
 * @param scores - NULL or float array that will contain the scores for cell (r, c) 
 * return 1 if the guess execution was successful, otherwise return -1
 */
int solveGuess(int** board, float threshold, int r, int c, float* scores);

/*
 * Get a copy of a board
 * @param board - Board to copy
 * return reference to the board copy
 */
int** copyBoard(int** board);

/*
 * Fill x random empty cells with legal values and then solve the board 
 * and then leave only y random cells 
 * @param board - Game board
 * @param x - number of cells to fill
 * @param y - number of cells to leave after solving the board
 * return 1 if execution was successful, otherwise return -1
 */
int fillXcells(int** board, int x, int y);

/*
 * Free board memory
 */
int freeBoard(int** board);

#endif
