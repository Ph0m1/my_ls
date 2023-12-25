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
#endif
// void parameter_handing(int argc,char *argv);
void do_a();
void do_l();
void do_R();
void do_t();
void do_r();
void do_i();
void do_s();
