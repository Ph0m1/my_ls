#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <ftw.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 800
// #define BUF_MAX 4096 * 128

#define a 0b1000000
#define l 0b0100000
#define R 0b0010000
#define t 0b0001000
#define r 0b0000100
#define I 0b0000010
#define s 0b0000001
#define Max_Dirnames 128
typedef struct
{
    char *dirnames[Max_Dirnames];
    int dirlen;
} DDir, *SqDir;
#endif
int parameter_handing(int argc,char *argv[],DDir* dn)
void do_a();
void do_l();
void do_R();
void do_t();
void do_r();
void do_i();
void do_s();
