#include "myls.h"



// 目录参数容器

// 处理参数
int parameter_handing(int argc,char *argv[],DDir* dn){
    int Vec = 0;
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
            strcpy(dn->dirnames[dn->dirlen], argv[i]);
            dn->dirlen++;
        }
    }
}
