#include "Stack.h"
#include "stdio.h"
#include "stdlib.h"

/* This is the Stack module used for simulating a recursive exhaustive algorithm in num_solution function */

StackL* stackL;

/* push a new iteration in the recursion into the stack */
int push(int i,int j,int* options, int optionsCount){
    StackL* stack = (StackL*)malloc(sizeof(StackL));
    if(stack==NULL){
        fprintf(stderr,"Coulnt allocate memory to stack");
        return -1;
    }
    stack->i=i;
    stack->j=j;
    stack->optionsCount=optionsCount;
    stack->options= options;
    if(stackL==NULL) {
        stackL = stack;
        stackL->prev=NULL;
    }
    else{
        stack->prev=stackL;
        stackL=stack;
    }
    return 1;
}

/* pop the last iteration in the recursion from the stack */
void pop(){
    StackL* stack = stackL;
    stackL=stackL->prev;
    freeStack(stack);
}

/* free a single iteration element in the stack */
void freeStack(StackL* stack){
    if(stack->options[0]>0)
        free(stack->options);
    free(stack);

}

/* free the whole stack */
int freeAllStack(){
    while (stackL!=NULL){
        pop();
    }
	return 0;   /*"must return a value"*/
}
