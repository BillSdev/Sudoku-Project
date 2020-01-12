
#ifndef FINALPROJECT_IOHANDLER_H
#define FINALPROJECT_IOHANDLER_H

/* This is the header file of IOHandler module. 
 * It will be used by the Game module to perform actions which require I/O
 */

extern int blockWidth;
extern int blockHeight;
extern int boardSize;

/*
 * Loading the file into our board
 * @param filePath - the path and name of the file to be loaded
 * return 1 if successfully loaded the file, otherwise return -1
 */
int loadBoard( char* filePath);

/*
 * Save the board into the file
 * @param filePath - the path and name of the file to be saved
 * return 1 if successfully saved the board, otherwise return -1
 */
int saveBoard(char* filePath);

#endif
