#define DECLARE_CLEANUP(name,type) static inline void cleanup_ ## name(type* v) { name(*v); }
DECLARE_CLEANUP(fclose,FILE*);
DECLARE_CLEANUP(close,in);
DECLARE_CLEANUP(free,void*);
#define cleanup(a) __attribute__((__cleanup__(cleanup_ ## a)))
