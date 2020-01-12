#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

/* This is the main Game module. MainAux uses this module to execute all user commands and run the game */

int** board;
int blockWidth=3;
int blockHeight=3;
int boardSize=9;
int markErrors=1;

/* 1 - is fixed , 0 - not fixed (or empty)*/
int isFixed(int r, int c) {
	if (board[r][c] < 0)
		return 1;
	return 0;
}

/* documented in header */
int loadEdit(char* filePath){
    int i,j;
    if(loadBoard(filePath)==-1){
        return -1;
    }
    for(i=0;i<boardSize;i++){
        for (j=0;j<boardSize;j++) {
                board[i][j]=-abs(board[i][j]);
        }
    }
    mark_errors(1);
    return 1;
}

/* documented in header */
int loadSolve(char* filePath){
    int i,j;
    int** newBoard;
    if(loadBoard(filePath)==-1){
        return -1;
    }
    mark_errors(1);

    /*
     * after we loaded in solve mode we need to check if there are 2 (or more) FIXED cells
     * who are neighbors and have the same value, in this case the file and board are
     * illegal - we set the mode to be Init and print appropriate message
     */
    newBoard = (int**)malloc(sizeof(int*)*boardSize);
    for(i=0;i<boardSize;i++)
        newBoard[i]=malloc(sizeof(int)*boardSize);
    /*
     * we create temporary board and set all the positive values to 0 so we will have only
     * fixed cells with values and check if the new board is valid.
     */
    for(i=0;i<boardSize;i++)
        for(j=0;j<boardSize;j++)
            if(board[i][j]<0)
                newBoard[i][j]=board[i][j];
            else
                newBoard[i][j]=0;

    if(validateCurrentStatus(newBoard)==-1){
        mode=Init;
        fprintf(stderr,"\nThe file have erroneous FIXED cells, file is invalid!");
        freeBoard(newBoard);
        return -1;
    }
    /* NEED TO VERIFY THERE IS VALID SOLUTION FOR THE BOARD WITH ONLY
     * THE FIXED CELLS */
    freeBoard(newBoard);
    return 1;
}

/* documented in header */
int save(char* filePath){
	int i, j, fullBoard;
	int** newBoard;
	if (mode == Edit) {
		fullBoard = 1;
		for (i = 0; i < boardSize; i++) {
			for (j = 0; j < boardSize; j++) {
				if (board[i][j] == 0)
					fullBoard = 0;
			}
		}
		if (fullBoard == 1) {
			printf("\nThe board is full, you need to have at least 1 empty cell!");
			return -1;
		}
		if (validateCurrentStatus(board) != 1){
			printf("\nThe board cannot be saved because it is erroneous!");
			return -1;
		}
		if (validate(board) != 1) {
			printf("\nThe board cannot be saved because it does not have a valid solution!");
			return -1;
		}
	}
	else if (mode == Solve) {
		/*
	 * after we loaded in solve mode we need to check if there are 2 (or more) FIXED cells
	 * who are neighbors and have the same value, in this case the file and board are
	 * illegal - we set the mode to be Init and print appropriate message
	 */
		newBoard = (int**)malloc(sizeof(int*)*boardSize);
		for (i = 0; i < boardSize; i++)
			newBoard[i] = malloc(sizeof(int)*boardSize);
		/*
		 * we create temporary board and set all the positive values to 0 so we will have only
		 * fixed cells with values and check if the new board is valid.
		 */
		for (i = 0; i < boardSize; i++)
			for (j = 0; j < boardSize; j++)
				if (board[i][j] < 0)
					newBoard[i][j] = board[i][j];
				else
					newBoard[i][j] = 0;
		if (validateCurrentStatus(newBoard) == -1) {
			if (validate(board) == -1) {
				printf("\nThe board have erroneous FIXED cells so cant be saved!");
				free(newBoard);
				return -1;
			}
		}
		free(newBoard);
	}
	if (saveBoard(filePath) == 1) {
		printf("\nThe board have been save into \"%s\".", filePath);
		return 1;
	}
	return -1;

}

/* documented in header */
int mark_errors(int newMark){
	if (newMark == 0 || newMark == 1) {
		markErrors = newMark;
		return 1;
	}
	return -1;
}

/* documented in header */
int checkWin(){
    int i,j;
    for(i=0;i<boardSize;i++) {
        for (j = 0; j < boardSize; j++) {
            if (board[i][j] == 0)
                return -1;
        }
    }
	if (validateCurrentStatus(board) == 1) {
		printf("\nCongratulations, you won! --- Setting mode back to Init...");
		mode = Init;
		return 1;
	}
	printf("\nThe solution is incorrect. You can undo your moves to continue solving.");
	return -1;
}

/* documented in header */
int setCell(int j,int i, int value){
    i--;
    j--;
    if(i<0 || j<0 || i>=boardSize || j>=boardSize){
        printf("\nGAME - setCell - Cell indexes most be between 1 to %d (included)",boardSize);
        return -1;
    }
    if(value<0 || value >boardSize){
        printf("\nGAME - setCell - Cell value most be between 0 to %d (included)",boardSize);
        return -1;
    }
    if(mode==Solve && board[i][j]<0){
        printf("\nCannot set cell (%d, %d) because it is fixed.",i+1,j+1);
        return -1;
    }

    addMove(board,board,Set,i,j,value);

    if(mode==Edit)
        board[i][j]=-abs(value);
	else 
		board[i][j] = value;
    return 1;

}

/* documented in header */
int guess(float threshold) {
	int** oldBoard;

	if (threshold < 0 || threshold > 1) {
		printf("\nThreshold is out of range (has to be between 0 and 1).");
		return -1;
	}
	
	oldBoard = copyBoard(board);
	if (oldBoard == NULL)
		return -1;

	if (solveGuess(board, threshold, 0, 0, NULL) != -1) 
		addMove(oldBoard, board, Guess, 0, 0, 0);
	else
		return -1;

	return 1;
}

/* documented in header */
int guess_hint(int x, int y) {
	int i;
	int** cpyBoard;
	float* scores = (float*)malloc(sizeof(float) * boardSize);

	if (isFixed(y-1, x-1) == 1) {
		printf("\nCould not give a guess because the cell (%d, %d) is fixed.", x, y);
		return -1;
	}
	if (board[y-1][x-1] != 0) {
		printf("\nCould not give a guess because the cell (%d, %d) already contains a value.", x, y);
		return -1;
	}
	if (validateCurrentStatus(board) == -1) {
		printf("\nCould not give a guess because the board is erroneous.");
		return -1;
	}

	cpyBoard = copyBoard(board);
	if (cpyBoard == NULL)
		return -1;

	if (solveGuess(board, 0, y - 1, x - 1, scores) == -1) {
		printf("\nerror getting scores for cell <%d, %d>", x, y);
		return -1;
	}
	printf("\nThe scores for cell <%d, %d> are : ", x, y);
	for (i = 0; i < boardSize; i++) {
		if(scores[i] > 0)
			printf("%d - %.2f \t", (i + 1), scores[i]);
	}

	return 1;
}

/* documented in header */
int generate(int x, int y) {
	int i, j, emptyCells;
	emptyCells = 0;
	/*
	 * Check if the board is erroneous
	 */
	if (validateCurrentStatus(board) == -1) {
		printf("\nThe board is erroneous.");
		return -1;
	}
	/*
	* Check if the board have valid solution
	*/
	if (validate(board) == -1) {
		return -1;
	}
	/*
	 * Check if there are at least x empty cells
	 */
	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
			if (board[i][j] == 0)
				emptyCells++;
	if (emptyCells < x) {
		printf("\nCant fill %d cells because there are only %d empty cells", x, emptyCells);
		return -1;
	}

	/*
	 * 1000 attempts to set X random empty cells with random valid option
	 * and then solve the board and empty all except y cells
	 */
	for (i = 0; i < 1000; i++) {
		if (fillXcells(board, x, y) == 1)
			return 1;
	}
	printf("\nError in the puzzle generator");
	return -1;

}

/* documented in header */
void printBoard(int** boardGame) {
	int i, j, k;
	printf("\n");
	for (i = 0; i < boardSize; i++) {
		if (i%blockHeight == 0) {
			for (k = 0; k < boardSize * 4 + boardSize / blockWidth + 1; k++)
				printf("-");
			printf("\n");
		}
		for (j = 0; j < boardSize; j++) {
			if (j == 0)
				printf("|");
			printf(" ");
			if (boardGame[i][j] == 0)
				printf("   ");
			else {
				if (mode == Solve && boardGame[i][j] < 0)
					printf("%2d.", abs(boardGame[i][j]));
				else {
					if ((mode == Edit || (mode == Solve && markErrors == 1)) && validateCell(boardGame, i, j) == -1)
						printf("%2d*", abs(boardGame[i][j]));
					else
						printf("%2d ", abs(boardGame[i][j]));
				}
			}
			if ((j + 1) % blockWidth == 0)
				printf("|");
		}
		printf("\n");
		if (i == boardSize - 1) {
			for (k = 0; k < boardSize * 4 + boardSize / blockWidth + 1; k++)
				printf("-");
			printf("\n");
		}

	}
}
