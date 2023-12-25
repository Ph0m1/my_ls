#include "myls.h"

#define BUF_MAX 4096*128

#define a 0b1000000
#define l 0b0100000
#define R 0b0010000
#define t 0b0001000
#define r 0b0000100
#define I 0b0000010
#define s 0b0000001

int Vec = 0;

// 目录参数容器
char *dirnames[4096 * 128];
int dirlen = 0;

int main(int argc, char *argv[])
{
    parameter_handing(argc,argv);
}

// 处理参数
void parameter_handing(int argc,char *argv[]){
    
    for (int i = 1; i < argc; i++)
    {
        //控制参数(以“-”开头)
        if (argv[i][0] == '-')
        {
            for (int j = 1; j < strlen(argv[i]); j++)
            {
                switch (argv[i][j])
                {
                case 'a':
                    Vec |= a;
                    break;
                case 'l':
                    Vec |= l;
                    break;
                case 'R':
                    Vec |= R;
                    break;
                case 't':
                    Vec |= t;
                    break;
                case 'r':
                    Vec |= r;
                    break;
                case 'i':
                    Vec |= I;
                    break;
                case 's':
                    Vec |= s;
                    break;
                default:
                    fprintf(stderr,"Usage is invalid: '-%c'", argv[i][j]);
                    return;
                }
            }
        }
        //非控制参数(路径)
        else
        {
            strcpy(dirnames[dirlen], argv[i]);
            dirlen++;
        }
    }
}

void do_a(){
    
}