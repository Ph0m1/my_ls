#ifndef ERROR_FUNCTIONS
#define ERROR_FUNCTIONS

void errMsg(const char*format, ...);

#ifdef _GNUC_

#define NORETURN _attribute_ ((_noreturn_))
#else
#define NORETURN
#endif

void errExit(const char *format, ...) NORETURN;

void err_exit(const char *format, ...) NORETURN;

void errExitEN(int errnum,const char *format, ...) NORETURN;

void fatal(const char *format, ...) NORETURN;

void usageErr(const char *format, ...) NORETURN;

void cmsdLineErr(const char *format, ...) NORETURN;

#endif
