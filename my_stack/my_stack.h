#ifndef _INC_STACK
#define _INC_STACK

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>

#define ERROR 0
#define OK 1
#define ElemeType_SStk int
#define MaxSize_SStk 100

typedef int status;

typedef struct
{
    ElemeType_SStk data[MaxSize_SStk];
    int top;
} SStkNode, *SqStack;

void InitStack(SqStack *s);
void DestoryStack(SqStack *s);
bool StackEmpty(SqStack s);
status Push(SqStack s, ElemeType_SStk e);
status Pop(SqStack s, ElemeType_SStk *e);
bool GetTop(SqStack s, ElemeType_SStk *e);

#endif