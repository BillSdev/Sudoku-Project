#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GurobiHelper.h"

/* This module handles all the Gurobi code .
 */

GRBenv   *env = NULL;
GRBmodel *model = NULL;
int       error = 0;
double*    sol;
int*       ind;
double*    val;
double*    obj;
char*      vtype;
int       optimstatus;
double    objval;

extern int blockWidth;
extern int blockHeight;
extern int boardSize;

extern int fillOptions(int validOptions[], int i, int j, int** boardSolver);

/* Free all memory related to Gurobi */
void freeGurobi() {
	/* IMPORTANT !!! - Free model and environment */
	free(sol);
	free(obj);
	free(vtype);
	free(val);
	free(ind);
	GRBfreemodel(model);
	GRBfreeenv(env);
}

/* Initializing Gurobi environment and model */
int initGurobiEnv() {
	/* Create environment - log file is mip1.log */
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("Gurobi Error : %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	/* STOP LOG TO CONSOLE */
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("Gurobi Error : %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	/* Create an empty model named "mip1" */
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("Gurobi Error : %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}
	
	return 1;
}

/* Here we set cell constraints - each cell can have only 1 value
 * which translates to : the sum of all boardSize variables related to a certain cell equals to 1
 * (in ILP it is equivalent to having a single value)
 */
static int setCellConstraints(int** board) {
	int i, j, k, optionsCount, boardSquare;
	int* options;
	boardSquare = boardSize * boardSize;
	for (i = 0; i < boardSize; i++) {
		for (j = 0; j < boardSize; j++) {
			/* for for a given cell <j+1, i+1> : */
			/* if it contains a value then all variables must remain 0 */
			if (board[i][j] != 0)
				continue;
			/* otherwise find possible legal values (options) 
			   and add a new constraint where the sum of all variables represented by the values is equal to 1 */
			options = (int *)malloc(sizeof(int) * (boardSize));
			for (k = 0; k < boardSize; k++)
				options[k] = k + 1;
			optionsCount = fillOptions(options, i, j, board);
			for (k = 0; k < optionsCount; k++) {
				ind[k] = i * boardSquare + j * boardSize + options[k] - 1;
				val[k] = 1;
			}
			error = GRBaddconstr(model, optionsCount, ind, val, GRB_EQUAL, 1, NULL);
			if (error) {
				printf("Gurobi Error : %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return -1;
			}
			free(options);
		}
	}
	return 1;
}

/* Here we set Value constraint - In each row , column and block , exactly one appearance of the each value is possible.*/
static int setValueConstraints(int** board) {
	int b, t, i, j, k, boardSquare, index, skip;
	boardSquare = boardSize * boardSize;
	/* Set the row constraints */
	for (k = 0; k < boardSize; k++) {
		for (i = 0; i < boardSize; i++) {
			/* For every value k and row i , we go over all the cells in row i
			   if a certain cell contains the value k+1, then the sum of variables
			   has to be 0 (hence skip = 0) because the value k+1 is already used in the row */
			skip = 1;
			for (j = 0; j < boardSize; j++) {
				ind[j] = i * boardSquare + j * boardSize + k;
				if (abs(board[i][j]) == k + 1) {
					val[j] = 0;
					skip = 0;
				}
				else
					val[j] = 1;
			}
			error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, skip, NULL);
			if (error) {
				printf("Gurobi Error : %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return -1;
			}
		}
	}

	/* Set the column constaints*/
	for (k = 0; k < boardSize; k++) {
		for (j = 0; j < boardSize; j++) {
			/* Same idea as in the row constraints */
			skip = 1;
			for (i = 0; i < boardSize; i++) {
				ind[i] = i * boardSquare + j * boardSize + k;
				if (abs(board[i][j]) == k + 1) {
					val[i] = 0;
					skip = 0;
				}
				else
					val[i] = 1;
			}
			error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, skip, NULL);
			if (error) {
				printf("Gurobi Error : %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
				return -1;
			}
		}
	}

	/* Set the block constraints */
	/* Same idea as in the row constrains (with the additional complexity of going through a block)*/
	for (b = 0; b < blockHeight; b++) {
		for (t = 0; t < blockWidth; t++) {
			for (k = 0; k < boardSize; k++) {
				index = 0;
				skip = 1;
				for (j = b * blockWidth; j < (b + 1)*blockWidth; j++) {
					for (i = t * blockHeight; i < (t + 1)*blockHeight; i++) {
						ind[index] = i * boardSquare + j * boardSize + k;
						if (abs(board[i][j]) == k + 1) {
							val[index] = 0;
							skip = 0;
						}
						else
							val[index] = 1;
						index++;
					}
				}
				error = GRBaddconstr(model, boardSize, ind, val, GRB_EQUAL, skip, NULL);
				if (error) {
					printf("Gurobi Error : %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
					return -1;
				}

			}
		}
	}
	return 1;
}

/* This set of constraints requires that all variables will be non negative */
static int setNonNegativeConstraints() {
	int i;
	for ( i = 0; i < boardSize * boardSize * boardSize; i++) {
		ind[0] = i;
		val[0] = 1;
		error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0, NULL);
		if (error) {
			printf("Gurobi Error : %d NonNegativeConstaint GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
			return -1;
		}
	}
	return 1;
}

/* Full documentation in GurobiHelper.h */
int validateILPSolver(int** board, int** newBoard) {
	int i, j, k, varCount, contraintCount, counter = 0;
	contraintCount = boardSize * boardSize;
	varCount = boardSize * boardSize * boardSize;

	if (initGurobiEnv() == -1)
		return -1;

	sol = (double *)malloc(sizeof(double) * varCount);
	obj = (double *)malloc(sizeof(double) * varCount);
	vtype = (char *)malloc(sizeof(char) * varCount);

	val = (double *)malloc(sizeof(double) * contraintCount);
	ind = (int *)malloc(sizeof(int) * contraintCount);
	/* coefficients - for x,y,z (cells 0,1,2 in "obj") */
	for (i = 0; i < boardSize; i++)
		for (j = 0; j < boardSize; j++)
			for (k = 1; k <= boardSize; k++) {
				if (board[i][j] != 0)
					obj[counter] = 0;
				else
					obj[counter] = 1;

				/* variables x,y,z (0,1,2) */
				/* variable types - for x,y,z (cells 0,1,2 in "vtype") */
				/* other options: GRB_INTEGER, GRB_CONTINUOUS */
				vtype[counter] = GRB_BINARY;
				counter++;
			}



	/* add variables to model */
	error = GRBaddvars(model, varCount, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if (error) {
		printf("Gurobi Error : %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("Gurobi Error : %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	/* update the model - to integrate new variables */

	error = GRBupdatemodel(model);
	if (error) {
		printf("\nGurobi Error : %d GRBupdatemodel(): %s", error, GRBgeterrormsg(env));
		return -1;
	}
	
	setCellConstraints(board);
	
	setValueConstraints(board);

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("\nGurobi Error : %d GRBoptimize(): %s", error, GRBgeterrormsg(env));
		return -1;
	}

	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("Gurobi Error : %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	/* Get solution information */

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("\nGurobi Error : %d GRBgetintattr(): %s", error, GRBgeterrormsg(env));
		return -1;
	}


	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		/* get the objective -- the optimal result of the function */
		error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
		if (error) {
			printf("\nGurobi Error : %d GRBgettdblattr(): %s", error, GRBgeterrormsg(env));
			return -1;
		}

		/* get the solution - the assignment to each variable */
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, varCount, sol);
		if (error) {
			printf("\nGurobi Error : %d GRBgetdblattrarray(): %s", error, GRBgeterrormsg(env));
			return -1;
		}

		for (i = 0; i < boardSize; i++) {
			for (j = 0; j < boardSize; j++) {
				if (board[i][j] != 0) {
					newBoard[i][j] = board[i][j];
					continue;
				}
				for (k = 0; k < boardSize; k++) {
					if (sol[i * contraintCount + j * boardSize + k] == 1) {
						newBoard[i][j] = k + 1;
						break;
					}
					if (k == boardSize - 1) {
						freeGurobi();
						return 0;
					}
				}
			}
		}

	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		return 0;
	}
	/* error or calculation stopped */
	else {
		printf("\nGurobi Error : Optimization was stopped early");
		return -1;
	}

	freeGurobi();
	return 1;
}

/* delete if new random weight method performs properly. */
/* Helper method for guessLP */
static void randomizeArray(int* arr) {
	int i, ran, temp;
	/*first fill array with all the values */
	for (i = 0; i < boardSize; i++) {
		arr[i] = i + 1;
	}
	/* now  we scramble the values */
	for (i = 0; i < boardSize-1; i++) {
		ran = rand() % (boardSize - i);
		temp = arr[ran];
		arr[ran] = arr[boardSize - i - 1];
		arr[boardSize - i - 1] = temp;
	}
}


/* This function returns using linear programming the score of each value k for each cell (i, j) on the board. 
   The higher the score, the higher the chance that guessing value k for cell (i, j) will be a valid guess.
   If the function was successful, lpBoard will contain all the scores. (lpBoard[i][j][k] - score of value (k+1) for cell (i+1, j+1))
   @param newBoard - the game board
		  lpBoard  - the board to which we insert the scores.
   @return : return 1 if function was successful , otherwise return -1.
   */
int guessLPSolver(int** board, float*** lpBoard) {
	int i, j, k, varCount, contraintCount, counter = 0;
	int *randWeight = (int*)malloc(sizeof(int) * boardSize);               
	contraintCount = boardSize * boardSize;
	varCount = boardSize * boardSize * boardSize;

	if (initGurobiEnv() == -1) {
		printf("\nGurobi Error : Problem initializing gurobi.");
		free(randWeight);
		return -1;
	}

	sol = (double *)malloc(sizeof(double) * varCount);
	obj = (double *)malloc(sizeof(double) * varCount);
	vtype = (char *)malloc(sizeof(char) * varCount);

	val = (double *)malloc(sizeof(double) * contraintCount);
	ind = (int *)malloc(sizeof(int) * contraintCount);

	/* coefficients - for x,y,z (cells 0,1,2 in "obj") */
	for (i = 0; i < boardSize; i++) {
		for (j = 0; j < boardSize; j++) {
			randomizeArray(randWeight);
			for (k = 1; k <= boardSize; k++) {
				if (board[i][j] != 0)
					obj[counter] = 0;
				else {
					obj[counter] = randWeight[k - 1];
					obj[counter] = rand() % (boardSize * boardSize) + 1;  /* overrides previous assignment */
				}
				vtype[counter] = GRB_CONTINUOUS;
				counter++;
			}
		}
	}
	/* add variables to model */
	error = GRBaddvars(model, varCount, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if (error) {
		printf("Gurobi Error : %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		free(randWeight);
		return -1;
	}

	/* Change objective sense to maximization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("Gurobi Error : %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		free(randWeight);
		return -1;
	}

	/* update the model - to integrate new variables */

	error = GRBupdatemodel(model);
	if (error) {
		printf("Gurobi Error : %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free(randWeight);
		return -1;
	}

	/* setting constraints */
	if (setCellConstraints(board) == -1 || setValueConstraints(board) == -1 || setNonNegativeConstraints() == -1) {
		free(randWeight);
		freeGurobi();
		return -1;
	}

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error) {
		printf("Gurobi Error : %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		free(randWeight);
		return -1;
	}
	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("Gurobi Error : %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		free(randWeight);
		return -1;
	}
	/* Get solution information */

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("Gurobi Error : %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		free(randWeight);
		return -1;
	}

	/* solution found */
	if (optimstatus == GRB_OPTIMAL) {
		/* get the objective -- the optimal result of the function */
		error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
		if (error) {
			printf("Gurobi Error : %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
			free(randWeight);
			return -1;
		}

		/* get the solution - the assignment to each variable */
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, varCount, sol);
		if (error) {
			printf("Gurobi Error : %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
			free(randWeight);
			return -1;
		}

		for (i = 0; i < boardSize; i++) {
			for (j = 0; j < boardSize; j++) {	
				for (k = 0; k < boardSize; k++) {
					lpBoard[i][j][k] = sol[i * boardSize * boardSize + j * boardSize + k];				
				}
			}
		}
		
	}
	/* no solution found */
	else if (optimstatus == GRB_INF_OR_UNBD) {
		printf("\nGurobi error : model is infeasible or unbounded.");
		free(randWeight);
		return -1;
	}
	/* error or calculation stopped */
	else {
		printf("\nGurobi Error : Optimization was stopped early");
		free(randWeight);
		return -1;
	}

	free(randWeight);  
	freeGurobi();
	return 1;
}