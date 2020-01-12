

#ifndef FINALPROJECT_STACK_H
#define FINALPROJECT_STACK_H

/* The header of the Stack module. Contains the Stack structure*/

/* Structure representing a node in the stack.
 * The node represents a single iteration in the recursion.
 * (contains the values in the iteration and a pointer back to the previous iteration (similar to return))
 */
typedef struct stack{
    int i;
    int j;
    int* options;
    int optionsCount;
    struct stack* prev;
} StackL;

/*
 * push new cell to the stack
 */
int push(int i,int j,int* options, int optionsCount);

/*
 * removing the top of the stack
 */
void pop();

/*
 * free the given stack cell
 */
void freeStack(StackL* stack);

/*
 * free the entire stack
 */
int freeAllStack();

#endif
