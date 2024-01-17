#include "my_stack.h"

void InitStack(SqStack *s)
{
    *s = (SqStack)malloc(sizeof(SStkNode));
    (*s)->top = -1;
}

void DestoryStack(SqStack *s)
{
    free(*s);
    *s = NULL;
}
status Push(SqStack s, ElemeType_SStk e)
{
    if (s->top == (MaxSize_SStk - 1))
        return ERROR;
    s->data[++s->top] = e;
    return OK;
}
status Pop(SqStack s, ElemeType_SStk *e)
{
    if (s->top == -1)
        return ERROR;
    *e = s->data[s->top--];
    return OK;
}
bool GetTop(SqStack s, ElemeType_SStk *e)
{
    if ((s->top == -1))
        return false;
    *e = s->data[s->top];
    return true;
}
bool StackEmpty(SqStack s)
{
    return (s->top == -1);
}