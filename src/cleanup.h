#define DECLARE_CLEANUP(name,type) static inline void cleanup_ ## name(type* v) { puts("cleanup " #name); name(*v); *v = 0; }
//#include <stdio.h>
//DECLARE_CLEANUP(fclose,FILE*);
//#include <unistd.h>
//DECLARE_CLEANUP(close,in);
//#include <stdlib.h>
//DECLARE_CLEANUP(free,void*);
//#include <dirent.h>
//DECLARE_CLEANUP(closedir,DIR*);
#define cleanup(a) __attribute__((__cleanup__(cleanup_ ## a)))
//cleanup(closedir) DIR* d = opendir(...);
