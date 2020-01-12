#include "stdio.h"
#include "stdlib.h"
#include "Solver.h"

/* This module handles any functions / tasks which requires heavy calculations and return an asnwer for Game module to use */

extern StackL* stackL;
extern int checkWin();
extern int printBoard(int** board);

/* Creates a copy of a given board.
 * Returns a pointer to the newly allocated copy of the board
 * If copying failed, return NULL
 */
int** copyBoard(int** board) {
	int** newBoard;
	int i, j;

	newBoard = (int**)malloc(boardSize * sizeof(int*));
	if (newBoard == NULL) {
		printf("\nCoulnt allocate memory to add board state - rows");
		return NULL;
	}
	for (i = 0; i < boardSize; i++) {
		newBoard[i] = (int*)malloc(boardSize * sizeof(int*));
		if (newBoard[i] == NULL) {
			printf("\nCoulnt allocate memory to add board state - columns");
			return NULL;
		}
	}
	for (i = 0; i < boardSize; i++) {
		for (j = 0; j < boardSize; j++) {
			newBoard[i][j] = board[i][j];
		}
	}
	return newBoard;
}


int fillOptions(int validOptions[], int i, int j, int** boardSolver) {
	int rowIndex, columnIndex, counter, cellValue = 0;
	counter = boardSize;

	/*
	 * Removing the numbers who appears in the row i or column j
	 * Idea: we go all over the cells in the respected row and column if they contain number we didnt removed them from the array
	 * yet we remove them (replace with 0) and removing from 1 from the counter
	 */
	for (rowIndex = 0; rowIndex < boardSize; rowIndex++) {
		cellValue = abs(boardSolver[i][rowIndex]);

		if (rowIndex != j && cellValue != 0 && validOptions[cellValue - 1] != 0) {

			validOptions[cellValue - 1] = 0;

			counter--;
		}
		cellValue = abs(boardSolver[rowIndex][j]);
		if (rowIndex != i && cellValue != 0 && validOptions[cellValue - 1] != 0) {
			validOptions[cellValue - 1] = 0;
			counter--;
		}
	}
	/*
	 * Removing the numbers who appears in the block of the cell i,j
	 * Idea: we go all over the cells in the block if they contain numbers we didnt removed them from the array
	 * yet we remove them (replace with 0) and removing from 1 from the counter
	 */
	for (rowIndex = (i / blockHeight)*blockHeight; rowIndex < (i / blockHeight)*blockHeight + blockHeight; rowIndex++)
		for (columnIndex = (j / blockWidth)*blockWidth; columnIndex < (j / blockWidth)*blockWidth + blockWidth; columnIndex++) {
			cellValue = abs(boardSolver[rowIndex][columnIndex]);
			if ((rowIndex != i || columnIndex != j) && cellValue != 0 && validOptions[cellValue - 1] != 0) {
				validOptions[cellValue - 1] = 0;

				counter--;
			}
		}

	/*
	 * We push all the numbers left (for every cell that contain number!=0 we look for the first cell from the left who contains 0 and replace them)
	 */
	for (rowIndex = 0; rowIndex < boardSize; rowIndex++)
		if (validOptions[rowIndex] != 0) {
			for (columnIndex = 0; columnIndex < rowIndex; columnIndex++)
				if (validOptions[columnIndex] == 0) {
					validOptions[columnIndex] = validOptions[rowIndex];
					validOptions[rowIndex] = 0;
					break;
				}

		}


	return counter;
}

int validateCell(int** board, int i, int j) {
	int rowIndex, columnIndex;
	/* check if there is a cell in the same row with the same value */
	for (rowIndex = 0; rowIndex < boardSize; rowIndex++) {
		if (rowIndex != j && board[i][j] != 0) {
			if (abs(board[i][rowIndex]) == abs(board[i][j])) {
				return -1;
			}
		}

	}

	/* check if there is a cell in the same column with the same value */
	for (rowIndex = 0; rowIndex < boardSize; rowIndex++) {
		if (rowIndex != i && board[i][j] != 0) {
			if (abs(board[rowIndex][j]) == abs(board[i][j])) {
				return -1;
			}
		}

	}

	/* check if there is a cell in the same block with the same value */
	for (rowIndex = (i / blockHeight)*blockHeight; rowIndex < (i / blockHeight)*blockHeight + blockHeight; rowIndex++)
		for (columnIndex = (j / blockWidth)*blockWidth; columnIndex < (j / blockWidth)*blockWidth + blockWidth; columnIndex++) {
			if ((rowIndex != i || columnIndex != j) && board[i][j] != 0)
				if (abs(board[rowIndex][columnIndex]) == abs(board[i][j])) {
					return -1;
				}
		}

	return 1;
}

/* Validate the board - for each cell we check there is no different cell with the same value in
 the same row / column / block. */
int validateCurrentStatus(int** board) {
	int i, j;
	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++) {
			if (validateCell(board, i, j) == -1)
				return -1;
		}
	return 1;
}

int num_solutions(int** board) {
	int i, j, counter, k, fillCount;
	int* options;
	/* Initialize a new board object so we can manipulate it to count how many
	 * possible solutions are available.*/
	int** newBoard;
	if (validateCurrentStatus(board) == -1)
		return 0;
	newBoard = (int**)malloc(boardSize * sizeof(int*));
	if (newBoard == NULL) {
		printf("\nCoulnt allocate memory to add board state - rows");
		return -1;
	}

	for (i = 0; i < boardSize; i++) {
		newBoard[i] = (int*)malloc(boardSize * sizeof(int*));
		if (newBoard[i] == NULL) {
			printf("\nCoulnt allocate memory to add board state - columns");
			return -1;
		}
	}
	/*
	 * We set in the new board cells the same value as in the original but always negative
	 * (so we will know to not change this cells when we manipulate the board)
	 */
	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
			newBoard[i][j] = -abs(board[i][j]);

	/*
	 * We add the first cell of the board (the cell in indexes 0,0) to be the first
	 * item in the stack.
	 * in general when we add item to the stack:
	 * IF the cell value in negative we simply add it to the stack and set the number of
	 * possible options in this cell to be 1.
	 * ELSE we find all the possible values for the cells and add item to the stack with this options,
	 * then setting the value in the board to be the last (which is also the largest) possible value.
	 *      IF there are no possible values we return 0
	 */
	i = j = 0;
	if (newBoard[i][j] < 0) {
		push(i, j, &(newBoard[i][j]), 1);
	}
	else {
		options = (int*)malloc(sizeof(int)*(boardSize));
		for (k = 0; k < boardSize; k++)
			options[k] = k + 1;
		fillCount = fillOptions(options, i, j, newBoard);
		if (fillCount <= 0) {
			freeAllStack();
			freeBoard(newBoard);
			return 0;
		}
		newBoard[i][j] = options[fillCount - 1];
		push(i, j, options, fillCount);

	}


	counter = 0;
	while (stackL != NULL) {
		i = stackL->i;
		j = stackL->j;

		if (j < boardSize - 1) {
			j++;
		}
		else if (i < boardSize - 1) {
			i++;
			j = 0;
		}
		else {
			/*
			 * We get here if we are in the last cell of the board.
			 * if we reached here -> we have valid value in each of the cells who apeared before and therefore
			 * the board have been solved.
			 * We increase the number of valid board by 1 and then pop cells from the stack until we reach a cell who have more then 1 valid options
			 * we decrease the number of options by 1 and set the cell to be the next option.
			 */
			counter++;
			while (stackL != NULL && (stackL->optionsCount <= 1 || stackL->options[0] < 0)) {
				/*
				 * if the cell value is positive then we set it to 0 - "reset the cell".
				 */
				if (stackL->options[0] > 0) {
					newBoard[stackL->i][stackL->j] = 0;
				}
				pop();
			}
			if (stackL != NULL) {
				stackL->optionsCount--;
				newBoard[stackL->i][stackL->j] = stackL->options[stackL->optionsCount - 1];

			}
			else {
				freeAllStack();
				freeBoard(newBoard);
				return counter;
			}
			continue;
		}
		if (newBoard[i][j] < 0) {
			push(i, j, &(newBoard[i][j]), 1);
			continue;
		}
		options = (int*)malloc(sizeof(int)*boardSize);
		for (k = 0; k < boardSize; k++)
			options[k] = k + 1;

		fillCount = fillOptions(options, i, j, newBoard);

		if (fillCount <= 0) {
			free(options);
			while ((stackL != NULL) && (stackL->optionsCount <= 1 || stackL->options[0] < 0)) {
				if (stackL->options[0] > 0) {
					newBoard[stackL->i][stackL->j] = 0;
				}
				pop();
			}

			if (stackL != NULL) {
				stackL->optionsCount--;
				newBoard[stackL->i][stackL->j] = stackL->options[stackL->optionsCount - 1];
			}
			else {
				freeAllStack();
				freeBoard(newBoard);
				return counter;
			}
		}
		else {
			newBoard[i][j] = options[fillCount - 1];
			push(i, j, options, fillCount);
		}

	}
	return 1;
}

int autofill(int** board) {
	int i, j, k, updateRequired;
	int** oldBoard;
	int *options;
	updateRequired = 1;  /* we need to count autofill as a move even if no cells were filled */
	oldBoard = copyBoard(board);
	if (oldBoard == NULL)
		return -1;

	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++) {
			if (board[i][j] != 0)
				continue;
			options = (int *)malloc(sizeof(int) * (boardSize));
			for (k = 0; k < boardSize; k++)
				options[k] = k + 1;

			if (fillOptions(options, i, j, oldBoard) == 1) {
				board[i][j] = options[0];
				printf("\nThe value in cell(%d, %d) was changed to %d", j + 1, i + 1, board[i][j]);
				updateRequired = 1;
			}
			free(options);
		}
	if (updateRequired == 1) {
		addMove(oldBoard, board, Autofill, 0, 0, 0);
	}
	freeBoard(oldBoard);
	return 1;
}

int freeBoard(int** board) {
	int i;
	for (i = 0; i < boardSize; i++)
		free(board[i]);
	free(board);
	return 0;   /*"must return a value"*/
}

int validate(int** board) {
	int i;
	int** newBoard;
	int ILPRes;
	if (validateCurrentStatus(board) == -1) {
		printf("\nThe board is erroneous.");
		return -1;
	}

	newBoard = (int**)malloc(boardSize * sizeof(int*));
	if (newBoard == NULL) {
		printf("\nCoulnt allocate memory to new board - rows.");
		return -1;
	}

	for (i = 0; i < boardSize; i++) {
		newBoard[i] = (int*)malloc(boardSize * sizeof(int));
		if (newBoard[i] == NULL) {
			printf("\nCoulnt allocate memory to new board - columns.");
			return -1;
		}
	}
	ILPRes = validateILPSolver(board, newBoard);
	freeBoard(newBoard);
	return ILPRes;
}

int hint(int x, int y, int** board) {
	int i;
	int** newBoard;

	if (validateCurrentStatus(board) == -1) {
		printf("\nThe board is erroneous.");
		return -1;
	}

	if (board[y - 1][x - 1] < 0) {
		printf("\nThe cell <%d,%d> is fixed.", x, y);
		return -1;
	}
	if (board[y - 1][x - 1] != 0) {
		printf("\nThe cell <%d,%d> already contains a value.", x, y);
		return -1;
	}

	newBoard = (int**)malloc(boardSize * sizeof(int*));
	if (newBoard == NULL) {
		printf("\nCoulnt allocate memory to new board - rows");
		return -1;
	}

	for (i = 0; i < boardSize; i++) {
		newBoard[i] = (int*)malloc(boardSize * sizeof(int));
		if (newBoard[i] == NULL) {
			printf("\nCoulnt allocate memory to new board - columns");
			return -1;
		}
	}


	if (validateILPSolver(board, newBoard) != 1) {
		free(newBoard);
		printf("\nThe board is unsolvable.");
		return -1;
	}
	printf("\nLegal value for the cell <%d,%d> is %d", x, y, newBoard[y - 1][x - 1]);
	free(newBoard);

	return 1;
}

/* return a random cell value based on the given scores of all the possible values for that cell
 * board - the board on which the scores where based.
 * i, j - cell coordinates
 * values - an array with all the scores for every value in order for the given (i, j) cell.
 * threshold - minimum score threshold for a value to be taken into account.
 */
static int randomValue(int** board, int i, int j, float* values, float threshold) {
	int k, temp, ran;
	for (k = 0; k < boardSize; k++) {
		temp = board[i][j];
		board[i][j] = k + 1;
		if (values[k] < threshold || validateCell(board, i, j) == -1)
			values[k] = 0;
		if (k > 0)
			values[k] = values[k] + values[k - 1];
		board[i][j] = temp;
	}

	if (values[boardSize - 1] == 0)
		return -1;

	ran = rand() % (int)(values[boardSize - 1] * 100) + 1;
	for (k = 0; k < boardSize; k++) {
		if (ran <= values[k] * 100)
			return k + 1;
	}

	return -1;
}

int solveGuess(int** board, float threshold, int r, int c, float* scores) {
	float*** lpBoard;
	int i, j, k, val;

	/*allocation memory for the board that will contain the scores of all possible values.*/
	/*might be redundant*/
	lpBoard = (float***)malloc(boardSize * sizeof(float**));
	if (lpBoard == NULL) {
		printf("\nMemory allocation error!");
		return -1;
	}
	for (i = 0; i < boardSize; i++) {
		lpBoard[i] = (float**)malloc(boardSize * sizeof(float*));
		if (lpBoard[i] == NULL) {
			printf("\nMemory allocation error!");
			return -1;
		}
		for (j = 0; j < boardSize; j++) {
			lpBoard[i][j] = (float*)malloc(boardSize * sizeof(float));
			if (lpBoard[i][j] == NULL) {
				printf("\nMemory allocation error!");
				return -1;
			}
		}
	}

	if (guessLPSolver(board, lpBoard) == -1) {
		/*free lpBoard*/
		for (i = 0; i < boardSize; i++) {
			for (j = 0; j < boardSize; j++) {
				free(lpBoard[i][j]);
			}
			free(lpBoard[i]);
		}
		free(lpBoard);
		return -1;
	}
	/* lpBoard is now filled with the scores of all possible values for all cells*/

	if (scores == NULL) {   /* in this case, we ran the guess command */
		for (k = 0; k < boardSize * boardSize; k++) {
			i = k / boardSize;
			j = k % boardSize;
			if ((val = randomValue(board, i, j, lpBoard[i][j], threshold)) != -1) {
				board[i][j] = val;
				printf("\nThe value in cell(%d, %d) was changed to %d", j + 1, i + 1, board[i][j]);
			}
		}
	}

	if (scores != NULL) {   /* here we ran the guess_hint command */
		for (i = 0; i < boardSize; i++)
			scores[i] = lpBoard[r][c][i];
	}
	
	/*free lpBoard*/
	for (i = 0; i < boardSize; i++) {
		for (j = 0; j < boardSize; j++) {
			free(lpBoard[i][j]);
		}
		free(lpBoard[i]);
	}
	free(lpBoard);

	return 1;
}

static int fillYcells(int** board, int** oldBoard, int y) {
	int i, j;
	/*
	 * newBoard - temp board to hold the solution to the board (if exists)
	 * oldBoard - temp board to hold the old board when we set the board
	 * to its new state so we can add to the MOVES LIST.
	 */
	int** newBoard;
	if (validateCurrentStatus(board) == -1) {
		return -1;
	}

	newBoard = (int**)malloc(boardSize * sizeof(int*));
	if (newBoard == NULL) {
		printf("\nCoulnt allocate memory to new board - rows.");
		return -1;
	}

	for (i = 0; i < boardSize; i++) {
		newBoard[i] = (int*)malloc(boardSize * sizeof(int));
		if (newBoard[i] == NULL) {
			printf("\nCoulnt allocate memory to new board - columns.");
			return -1;
		}
	}

	if (validateILPSolver(board, newBoard) == -1) {
		freeBoard(newBoard);
		return -1;
	}
	/*
	 * We save our board state into the oldBoard variable and set 0's to out current board.
	 * we then fill y random empty cells in our current board to be equal to the value
	 * in the same cell at newBoard (who hold our solution).
	 */
	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++) {
			board[i][j] = 0;
		}
	while (y > 0) {
		i = rand() % boardSize;
		j = rand() % boardSize;
		if (board[i][j] != 0)
			continue;
		board[i][j] = -abs(newBoard[i][j]);
		y--;
	}
	addMove(oldBoard, board, Generate, 0, 0, 0);
	freeBoard(newBoard);

	return 1;
}


int fillXcells(int** board, int x, int y) {
	int i, j, k, filled, optionsCount;
	int** oldBoard;
	int* options;
	int* saveI;
	int* saveJ;
	oldBoard = (int**)malloc(boardSize * sizeof(int*));
	if (oldBoard == NULL) {
		printf("\nCoulnt allocate memory to new board - rows.");
		return -1;
	}
	for (i = 0; i < boardSize; i++) {
		oldBoard[i] = (int*)malloc(boardSize * sizeof(int));
		if (oldBoard[i] == NULL) {
			printf("\nCoulnt allocate memory to new board - columns.");
			return -1;
		}
	}
	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++) {
			oldBoard[i][j] = board[i][j];
		}
	options = (int*)malloc(sizeof(int)*boardSize);
	saveI = (int*)malloc(sizeof(int)*x);
	saveJ = (int*)malloc(sizeof(int)*x);
	filled = 0;
	/*
	 * Try to fill X empty cells with valid values
	 */
	while (filled < x) {
		i = rand() % boardSize;
		j = rand() % boardSize;
		if (board[i][j] != 0)
			continue;
		for (k = 0; k < boardSize; k++)
			options[k] = k + 1;
		optionsCount = fillOptions(options, i, j, board);
		/*
		 * We got to an empty cell with no valid options
		 */
		if (optionsCount == 0) {
			free(options);
			for (k = 0; k < filled; k++)
				board[saveI[k]][saveJ[k]] = 0;
			free(saveI);
			free(saveJ);
			freeBoard(oldBoard);
			return -1;
		}
		if (optionsCount == 1)
			board[i][j] = options[0];
		else
			board[i][j] = options[rand() % optionsCount];
		saveI[filled] = i;
		saveJ[filled] = j;
		filled++;
	}


	free(options);
	/*
	 * We filled x empty cells succefully - try to solve the board and empty all except
	 * y cells who will be filled.
	 */
	if (fillYcells(board, oldBoard, y) == -1) {
		for (k = 0; k < filled; k++)
			board[saveI[k]][saveJ[k]] = 0;
		free(saveI);
		free(saveJ);
		freeBoard(oldBoard);
		return -1;
	}
	freeBoard(oldBoard);
	free(saveI);
	free(saveJ);

	return 1;
}



