
#ifndef FINALPROJECT_GUROBIHELPER_H
#define FINALPROJECT_GUROBIHELPER_H
#include "gurobi_c.h"

/* Header for GurobiHelper module
 * Provides a function to solve ILP problems needed for board validation
 * and LP problems needed for guessing scores of each value.
 */

/*
 * initialize gurobi
 * return 1 if initialization was successful, otherwise return -1.
 */
int initGurobiEnv();

/*
 * Checks using ILP whether the board has a solution or is unsolvable.
 * @param board - the board to check its validity.
 * @param newBoard - if a solution will be found, it will be inserted into newBoard.
 * return 1 if a solutin was found, return 0 if board is unsolvable, return -1 if gurobi error occured.
 */
int validateILPSolver(int** board, int** newBoard);

/*
 * Guess all the scores of all values for every cell in the board.
 * @param board - the game board
 * @param lpBoard - the 3d board to which we insert the score of all the values for every cell.
 * return 1 if successfully given all the scores, otherwise return -1.
 */
int guessLPSolver(int** board, float*** lpBoard);
#endif
