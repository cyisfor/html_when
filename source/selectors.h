#include <libxml/HTMLparser.h>

enum directions { DOWN, /*LEFT,*/ RIGHT, UP } ;
/* Theseus strategy: if last direction was UP, check RIGHT, UP, then done
	 if last direction was RIGHT, check DOWN, RIGHT, UP
	 if last direction was DOWN, check DOWN, RIGHT, UP
	 keep right hand on the wall!
	 DOWN should be default
*/

struct Selector {
	xmlNode* next;
	const char* name;
	enum directions last;
};

void find_start(struct Selector* s, xmlNode* top, const char* name);
xmlNode* find_next(struct Selector* pos);
