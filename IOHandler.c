#include "IOHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This module directly handles everything related to reading from / writing to files */

extern int** board;

/*
 * clear the game board
 */
int clearGame(){
    int i;
    if(board!=NULL) {
        for (i = 0; i < boardSize; i++)
            free(board[i]);
        free(board);
        board=NULL;
    }
    blockWidth=blockHeight=3;
    boardSize=9;
	return 0;    /*"must return a value"*/
}

/*
 * Initialize the board
 */
int initBoard(){
    int i,j;
    board=(int**)malloc(boardSize*sizeof(int*));
    if(board==NULL){
        fprintf(stderr,"Coulnt allocate memory to add board state - rows");
        return -1;
    }
    for(i=0;i<boardSize;i++){
        board[i]=(int*)malloc(boardSize*sizeof(int));

        if(board[i]==NULL){
            fprintf(stderr,"Coulnt allocate memory to add board state - columns");
            return -1;
        }
    }
    for(i=0;i<boardSize;i++){
        for (j=0;j<boardSize;j++) {
            board[i][j]=0;
        }
    }
    return 1;
}

/* Full documentation in IOHandler.h */
int loadBoard( char* filePath){
    int input, i, j;
    char nextChar;
    FILE* loadFile;
    clearGame();
    if((loadFile = fopen(filePath,"r"))==NULL){
        fprintf(stderr, "\nFailed in opening file to read");
        /* MAKE SEGMENTATION FUALT ON LINUX - HAPPENS WHEN FILE DOESNT EXIST */
        /*fclose(loadFile);*/
        return -1;
    }
    while ((nextChar=fgetc(loadFile))!=EOF){
        if(!(nextChar==' '||nextChar=='\n'||nextChar=='\r'||
                nextChar=='\t'||nextChar=='\v'||nextChar=='\f'||nextChar==EOF)) {
          if(nextChar!='.'&&(nextChar<'0'||nextChar>'9')){
              fprintf(stderr,"\nIOHandler - loadBoard - corrupted file, contain invalid character '%c'",nextChar);
              fclose(loadFile);
              return -1;
          }
        }
    }
    rewind(loadFile);
    /*
     * Loading the first 2 params from the file who suppose to be 1. the block width 2. the block height
     */
    if(fscanf(loadFile,"%d",&input)!=1){
        fprintf(stderr,"\nFailed in reading first param from file");
        fclose(loadFile);
        return -1;
    }
    blockWidth = input;
    if(fscanf(loadFile,"%d",&input)!=1){
        fprintf(stderr,"\nFailed in reading second param from file");
        fclose(loadFile);
        return -1;
    }
    blockHeight = input;

    boardSize=blockHeight*blockWidth;
    if(boardSize<=1||boardSize>=100){
        fprintf(stderr,"\nCorrupted file, board size cant be bigger then 99 or less 2,"
                      "file board size is: %d",
                      boardSize);
        fclose(loadFile);
        return -1;
    }

    /*
     * we initialize the board with the board and block sizes we got from the first line in the file
     */
    if(initBoard()!=1){
        fprintf(stderr,"\nFailed to initialize the board");
        fclose(loadFile);
        return -1;
    }
    i=j=0;

    /* while condition - we successfully read integer from the file */
    while (fscanf(loadFile,"%d",&input)==1 ){
        /*if the file has more numbers than it should we return -1
         * which indicates an error (in this case corrupted or invalid file)*/
        if(i >= boardSize || j>= boardSize){
            fprintf(stderr,"\nIOHandler - loadBoard - file is corrupted or invalid (have too many variables)\n");
            fclose(loadFile);
            return -1;
        }

        /* if the number input is followed by a '.' we set it as negative number to
         * indicate it is fixed cell*/
        if((nextChar=fgetc(loadFile))=='.')
            board[i][j]=-input;
        /*else if(nextChar==' '||nextChar==""||nextChar=='\n'||nextChar=='\r'||      ---  ? nextChar == ""  ~  nextChar == '\0' ? */    
		else if (nextChar == ' ' || nextChar == '\0' || nextChar == '\n' || nextChar == '\r' ||
        nextChar=='\t'||nextChar=='\v'||nextChar=='\f'||nextChar==EOF) {
            board[i][j] = input;
        }else{
            fprintf(stderr,"\nIOHandler - loadBoard - corrupted file, contain invalid character '%c'",nextChar);
            fclose(loadFile);
            return -1;
        }

        if(board[i][j]<-boardSize||board[i][j]>boardSize){
            fprintf(stderr,"\nIOHandler - loadBoard - valid board values are between 0 to %d,"
                           "received %d\n",boardSize,board[i][j]);
            fclose(loadFile);
            return -1;
        }


        /* we move the indexes (i,j) to the next cell to be filled */
        if(j==boardSize-1) {
            j = 0;
            i++;
        }
        else
            j++;
    }
    if(i<boardSize){
        fclose(loadFile);
        fprintf(stderr,"\nIOHandler - loadBoard - file is corrupted or invalid (dont have enough variables)\n");
        return -1;
    }
    fclose(loadFile);
    return 1;
}
int counterF=1;

/* Full documentation in IOHandler.h */
int saveBoard( char* filePath){
    int i,j;
    FILE* fileSave;

    if((fileSave=fopen(filePath,"w"))==NULL) {
        fprintf(stderr,"\nCould not save file (failed to open / create file to save into)");
        return -1;
    }
    /* we write in the first line the 2 block param (width and height) */
    fprintf(fileSave,"%d %d\n",blockWidth,blockHeight);

    for(i=0;i<boardSize;i++){
        for (j=0;j<boardSize;j++) {
            /* we write the cell absolute value - if its negative it follows a '.' */
            fprintf(fileSave,"%d",abs(board[i][j]));
			if (board[i][j] < 0)
				fprintf(fileSave, ".");
            if(j<boardSize-1)
                fprintf(fileSave," ");
        }
        fprintf(fileSave,"\n");
    }
    if(fclose(fileSave)==EOF){
        fprintf(stderr,"\nThere was a problem with saving the board into the file");
        return -1;
    }


    return 1;
}
