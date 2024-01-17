#include "myls.h"
int Vec = 0;
int main(int argc, char *argv[])
{
    SqDir dn;
    dn = (SqDir)malloc(sizeof(DDir));
    dn->dirlen = 0;
    Vec = parameter_handing(argc, argv, dn);
}