#include "gumbo.h"

enum directions { DOWN, /*LEFT,*/ RIGHT, UP } ;
/* Theseus strategy: if last direction was UP, check RIGHT, UP, then done
	 if last direction was RIGHT, check DOWN, RIGHT, UP
	 if last direction was DOWN, check DOWN, RIGHT, UP
	 keep right hand on the wall!
	 DOWN should be default
*/

// for moving up, has to keep a stack of cpos's...
// can't use call stack because next gets re-called... function pointers?
//typedef bool (*Checker)(GumboNode*,void*);
struct Selector {
	int* data; // make sure to init = {}
	size_t n;
	size_t space;
	void* udata;
	GumboTag check;
	GumboNode* next;
	enum directions last;
};

void find_start(struct Selector* s, GumboNode* top, GumboTag check, void* udata);
GumboNode* find_next(struct Selector* pos);

