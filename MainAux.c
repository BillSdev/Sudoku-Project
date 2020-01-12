#include "MainAux.h"

extern int** board;
extern int markErrors;

static int running;

void init() {
	running = 1;
	mode = Init;
	printf("Welcome To console Sudoku");
}

int edit(char* fileName) {
	/*
	 * we first make sure the move list is empty (if its not we empty it)
	 * then we try to load the file in edit mode and initialize new moves list.
	 */
	if (loadEdit(fileName) == -1)
		return -1;
	freeAllList();
	freeAllStack();
	if (addMove(board, board, Default, 0, 0, 0) == -1) {
		return -1;
	}

	mode = Edit;
	return 1;
}

int editDefault() {
	/*
	 * we first make sure the move list is empty (if its not we empty it)
	 * then we try to initialize the game in edit mode and initialize new moves list.
	 */
	freeAllList();
	freeAllStack();
	blockWidth = blockHeight = 3;
	boardSize = 9;
	if (initBoard() == -1 ||addMove(board,board,Default,0,0,0) == -1) {
		return -1;
	}
	mode = Edit;
	return 1;
}

int solve(char* fileName) {
	/*
	 * we first make sure the move list is empty (if its not we empty it)
	 * then we try to load the file in solve mode and initialize new moves list.
	 */
	if (loadSolve(fileName) == -1)
		return -1;
	freeAllList();
	freeAllStack();
	if (addMove(board, board, Default, 0, 0, 0) == -1) {
		return -1;
	}
	mode = Solve;
	return 1;
}

/* Basic argument value checks are done here for a given command. */
int runCommand(Command* command) {
	if (command->commandType == SOLVE)
		return solve(command->filePath);
	else if (command->commandType == EDIT) {
		if (command->argnumReceived == 0)
			return editDefault();
		return edit(command->filePath);
	}
	else if (command->commandType == MARK_ERRORS) {
		if (command->firstNotNumber == 1) {
			printf("\nThe parameter is not a number. \nPlease use '0' to hide errors and '1' to mark errors.");
			return -1;
		}
		if (mark_errors(command->arguments[0]) == -1) {
			printf("\nParameter is out of range for mark_errors command. \nPlease use '0' to hide errors and '1' to mark errors.");
			return -1;
		}
		return 1;
	}
	else if (command->commandType == PRINT_BOARD) {
		printGame();
		return 1;
	}
	else if (command->commandType == GUESS) {
		if (command->firstNotNumber == 1) {
			printf("\nThe given threshold is not a number. Please use a number between 0 and 1.");
			return -1;
		}

		if (command->threshold < 0 || command->threshold > 1) {
			printf("\nInvalid input, Guess threshold has to be between 0 and 1.");
			return -1;
		}
		if (guess(command->threshold) == -1) {
			printf("\nProblem guessing");
			return -1;
		}
		return 1;
	}
	else if (command->commandType == GENERATE) {
		if (command->firstNotNumber == 1)
			printf("\nFirst parameter is not a number. Please enter a number between 1 and %d", boardSize*boardSize);
		else if (command->arguments[0] < 0 || command->arguments[0] > boardSize*boardSize)
			printf("\nFirst parameter is out of range. Please enter a number between 1 and %d", boardSize*boardSize);
		else if(command->firstNotNumber == 2)
			printf("\nSecond parameter is not a number. Please enter a number between 1 and %d", boardSize*boardSize);
		else if (command->arguments[1] < 0 || command->arguments[1] > boardSize*boardSize)
			printf("\nSecond parameter is out of range. Please enter a number between 1 and %d", boardSize*boardSize);
		else
			return generate(command->arguments[0], command->arguments[1]);  
		return -1;
	}
	else if (command->commandType == UNDO) {
		return undo(board);
	}
	else if (command->commandType == REDO) {
		return redo(board);
	}
	else if (command->commandType == SET) {
		char message[300];
		strcpy(message, "\nCannot execute set command because ");
		if (command->firstNotNumber == 1)
			strcat(message, "the X coordinate is not a number.");
		else if (command->arguments[0] < 1 || command->arguments[0] > boardSize)
			strcat(message, "the X coordiante is out of range.");
		else if (command->firstNotNumber == 2)
			strcat(message, "the Y coordinate is not a number.");
		else if (command->arguments[1] < 1 || command->arguments[1] > boardSize)
			strcat(message, "the Y coordinate is out of range.");
		else if (command->firstNotNumber == 3)
			strcat(message, "the cell value is not a number.");
		else if (command->arguments[2] < 0 || command->arguments[2] > boardSize)
			strcat(message, "the cell value is out of range.");
		else
			return setCell(command->arguments[0], command->arguments[1], command->arguments[2]);    /*additional arg checks are done inside setCell*/
		printf("%s", message);
		printf("\nTry : Set X Y Z  where X and Y are integers between 1 and %d and Z is an integer between 0 and %d.", boardSize*boardSize, boardSize*boardSize);
		return -1;
	}
	else if (command->commandType == HINT) {
		if (command->firstNotNumber == 1)
			printf("\nFirst parameter is not a number. Please enter a number between 1 and %d", boardSize);
		else if (command->arguments[0] < 0 || command->arguments[0] > boardSize)
			printf("\nFirst parameter is out of range. Please enter a number between 1 and %d", boardSize);
		else if (command->firstNotNumber == 2)
			printf("\nSecond parameter is not a number. Please enter a number between 1 and %d", boardSize);
		else if (command->arguments[1] < 0 || command->arguments[1] > boardSize)
			printf("\nSecond parameter is out of range. Please enter a number between 1 and %d", boardSize);
		else
			return hint(command->arguments[0], command->arguments[1], board);
		return -1;
	}
	else if (command->commandType == VALIDATE) {
		int res = validate(board);
		if (res == 1) {
			printf("\nThe board is solvable.");
			return 1;
		}
		if (res == 0)
			printf("\nThe board is not solvable.");
		return -1;
	}
	else if (command->commandType == RESET) {
		return resetBoard(board);
	}
	else if (command->commandType == AUTOFILL) {
		return autofill(board);
	}
	else if (command->commandType == SAVE) {
		return save(command->filePath);
	}
	else if (command->commandType == GUESS_HINT) {
		if (command->firstNotNumber == 1)
			printf("\nFirst parameter is not a number. Please enter a number between 1 and %d", boardSize);
		else if (command->arguments[0] < 0 || command->arguments[0] > boardSize)
			printf("\nFirst parameter is out of range. Please enter a number between 1 and %d", boardSize);
		else if (command->firstNotNumber == 2)
			printf("\nSecond parameter is not a number. Please enter a number between 1 and %d", boardSize);
		else if (command->arguments[1] < 0 || command->arguments[1] > boardSize)
			printf("\nSecond parameter is out of range. Please enter a number between 1 and %d", boardSize);
		else
			return guess_hint(command->arguments[0], command->arguments[1]);
		return -1;
	}
	else if (command->commandType == NUM_SOLUTIONS) {
		int res = num_solutions(board);
		if (res == -1)
			return -1;
		printf("\nThere are %d possible solutions for the current board.", res);
		return 1;
	}
	else if (command->commandType == EXIT) {
		printf("\nExiting program...");
		running = 0;
	}

	return -1;
}

void modeToString(enum Mode mode, char *str) {
	if (mode == Init)
		strcpy(str, "[INIT]");
	if (mode == Solve)
		strcpy(str, "[SOLVE]");
	if (mode == Edit)
		strcpy(str, "[EDIT]");
}

int checkCommandValidity(Command *command) {
	char message[200];

	/*first checking if the command is invalid (does not exist or called in a wrong mode).*/
	if (command->commandType == INVALID) {
		printf("\nCommand name is invalid");
		return 0;
	}
	if (command->gameStateAvailability[mode] == 0) {
		char modeStr[10];
		modeToString(mode, modeStr);
		strcpy(message, "Cannot execute \"");
		strcat(message, command->commandName);
		strcat(message, "\" in mode  ");
		strcat(message, modeStr);
		strcat(message, ".\nThis action is only available in the following modes :   ");
		if (command->gameStateAvailability[Init] == 1)
			strcat(message, "[INIT]   ");
		if (command->gameStateAvailability[Solve] == 1)
			strcat(message, "[SOLVE]  ");
		if (command->gameStateAvailability[Edit] == 1)
			strcat(message, "[EDIT]   ");
		strcat(message, "\n");
		printf("%s",message);
		return 0;
	}

	/* checking if the number of arguments given is correct */
	if (command->argnum < command->argnumReceived) {
		printf("\nToo many arguments, the command \"%s\" requires %d arguments.", command->commandName, command->argnum);
		return 0;
	}
	if (command->argnum > command->argnumReceived) {
		if (command->commandType == EDIT && command->argnumReceived == 0) {

		}
		else {
			printf("\nNot enough arguments, the command \"%s\" requires %d arguments.", command->commandName, command->argnum);
			return 0;
		}
	}
	return 1;

}

void freeAll() {
	freeAllList();
	freeAllStack();
	freeBoard(board);
}

void printGame() {
	printBoard(board);
}

int isRunning() {
	if (running == 1) {
		printf("\nPlease enter a command : \n\t>");
		return 1;
	}
	return 0;
}


