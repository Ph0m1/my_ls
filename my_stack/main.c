#include "my_stack.h"

int main(int argc,char* argv[]){
    SqStack s;
    ElemeType_SStk e;
    InitStack(&s);
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++){
        scanf("%d", &e);
        Push(s, e);
    }
    if(GetTop(s,&e))
        ;
    printf("TOP: %d\n", e);
    for (int i = 0; i < n; i++)
    {
        if(Pop(s, &e))
        printf("%d ", e);
    }
    DestoryStack(&s);
    return 0;
}