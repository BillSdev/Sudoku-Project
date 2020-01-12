

#ifndef FINALPROJECT_LIST_H
#define FINALPROJECT_LIST_H

/* Header of the List module.
 * Provides visibility to move related enums, structures and to varies list actions.
 */

/*
 * enum representing all the possible moves
 */
typedef enum MoveType{Set,Autofill,Generate,Guess,Default}MoveType;

/*
 * Structure representing a move
 */
typedef struct Move {
    MoveType type;
    struct Move* next;
    struct Move* prev;
    struct MoveValue* movesList;

}Move;

/*
 * Structure which holds the value information of a move
 */
typedef struct MoveValue {
    int i;
    int j;
    int undoValue;
    int redoValue;
    struct MoveValue* next;
}MoveValue;

/*
 * Add a new move to the move list.
 * @param board - The game board
 * @param newBoard - Game board after a move was successfully executed
 * @param type - Move type
 * @param i - Row of affected cell
 * @param j - Column of affected cell
 * @param value - New value of affected cell
 * return 1 if the move was successfully added, otherwise return -1
 */
int addMove(int** board,int** newBoard,MoveType type,int i,int j,int value);

/*
 * Undo the last move
 * @param board - The game board
 * return 1 if successfuly undid the last move, otherwise return -1
 */
int undo(int** board);

/*
 * Redo the last undone move
 * @param board - The game board
 * return 1 if successfully redid the last undone move, otherwise return -1
 */
int redo(int** board);

/*
 * Return the board to its original state
 * @param board - The game board 
 * return 1 if reset was successful, otherwise return -1
 */
int resetBoard(int** board);

/*
 * Free move list resources
 */
void freeAllList();

#endif
