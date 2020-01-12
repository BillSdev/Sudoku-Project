#include <stdlib.h>
#include <stdio.h>
#include "List.h"

/* This is the module representing the move list 
 * The list is made of Move nodes, and every Move node contains a chain of MoveValues
 * representing the changes made in a single move
 */

static Move* currentNode;
extern int boardSize;

/*
 * free the given Move and all the Moves that have been added after him.
 * @param Move to be freed
 */
void freeList(Move* move){
    MoveValue* next;
    if(move==NULL)
        return;

    while (move->movesList!=NULL){
        next=move->movesList->next;
        free(move->movesList);
        move->movesList=next;
    }
    freeList(move->next);
    free(move);
}

/*
 * Update the board according to the current head of the list
 * if mode = 1 then we undo , otherwise we redo
 */
static int updateBoard(int** board,int mode){

    MoveValue* moveValue;
    if(currentNode==NULL){
        printf("\nCant update the board - NULL head");
        return -1;
    }

    moveValue=currentNode->movesList;
    while (moveValue!=NULL){
        if(mode==1){
            printf("\nThe value in cell(%d, %d) was changed from %d to %d",
				moveValue->j + 1, moveValue->i + 1, abs(board[moveValue->i][moveValue->j]), abs(moveValue->undoValue));
            board[moveValue->i][moveValue->j]=moveValue->undoValue;
        }
        else{
            printf("\nThe value in cell(%d, %d) was changed from %d to %d",
				moveValue->j + 1, moveValue->i + 1, abs(board[moveValue->i][moveValue->j]), abs(moveValue->redoValue));
            board[moveValue->i][moveValue->j]=moveValue->redoValue;
        }
        moveValue=moveValue->next;
    }

    return 1;
}


/* we move the list head to the previous move in the list (if possible) and update the board
 *          - Full documentation in List.h -
 */
int undo(int** board){

    if(currentNode==NULL||currentNode->type==Default){
        printf("No previous moves\n");
        return 0;
    }
    updateBoard(board,1);
    if(currentNode->prev!=NULL) {
        currentNode = currentNode->prev;
    }

    return 1;
}


/* we move the list head to the next move in the list (if possible) and update the board 
 *          - Full documentation in List.h -
 */
int redo(int** board){

    if(currentNode==NULL || currentNode->next==NULL) {
        printf("No redo moves\n");
        return 0;
    }

    if(currentNode->next!=NULL) {
        currentNode = currentNode->next;
        updateBoard(board, 0);
    }


    return 1;
}


/* we move the list head to the previous move until we reach the first move in the
 * list (if possible) and update the board 
 *          - Full documentation in List.h -
 */
int resetBoard(int** board){
    while (currentNode!=NULL&&currentNode->prev!=NULL&&currentNode->prev->type!=Default) {
        undo(board);
    }
    undo(board);
    return 1;
}


/*we free the entire list by using the freeList method on the first move headed to the list.*/
void freeAllList(){

    if(currentNode==NULL)
        return;
    while (currentNode->prev!=NULL) {
        currentNode = currentNode->prev;
    }
    freeList(currentNode);
    currentNode=NULL;
}

/*
 * we free all the nodes that were added after the list head move
 * and then we add new move to the list and setting the head to be this move.
 *          - Full documentation in List.h -
 */
int addMove(int** board,int** newBoard,MoveType type,int i,int j,int value){
    MoveValue* nextValue;

    Move* nextNode = (Move *) malloc(sizeof(Move));
    if (nextNode == NULL) {
        fprintf(stderr, "Coulnt allocate memory to add Move\n");
        return -1;
    }
    nextNode->movesList=NULL;

    if(type==Set){
        MoveValue* moveValue = (MoveValue*)malloc(sizeof(MoveValue));
        moveValue->undoValue=board[i][j];
        moveValue->redoValue=value;
        moveValue->i=i;
        moveValue->j=j;
        moveValue->next=NULL;
        nextNode->movesList=moveValue;

    }else{
        for(i=0;i<boardSize;i++)
            for(j=0;j<boardSize;j++)
				if (type == Default || abs(board[i][j]) != abs(newBoard[i][j])) {
                    if(nextNode->movesList==NULL) {
                        nextNode->movesList = (MoveValue *) malloc(sizeof(MoveValue));
                        nextValue = nextNode->movesList;

                    }else {

                        nextValue = nextNode->movesList;
                        while (nextValue->next != NULL) {
                            nextValue = nextValue->next;
                        }
                        nextValue->next= (MoveValue*)malloc(sizeof(MoveValue));
                        nextValue=nextValue->next;

                    }
                    if(board!=NULL)
                        nextValue->undoValue=board[i][j];
                    if(newBoard!=NULL)
                        nextValue->redoValue=newBoard[i][j];

                    nextValue->i=i;
                    nextValue->j=j;
                    nextValue->next=NULL;


                }
    }
    nextNode->type=type;
    nextNode->next= NULL;
    nextNode->prev= (currentNode!=NULL)?currentNode:NULL;

    /*
     * if the head is not null we free all the moves that were added after the head
     * and then set the next move of it to be the move we just got.
     */
    if(currentNode!=NULL){
        freeList(currentNode->next);
        currentNode->next=nextNode;
    }

    currentNode=nextNode;
    return 1;
}
