#include "selectors.h"
#include <stdlib.h> // malloc...
#include <error.h>
#include <assert.h>
#include <stdio.h>

static void find_destroy(struct Selector* s) {
}

void find_start(struct Selector* s, htmlNode* top, const char* check) {
	assert(s->last == DOWN);
	assert(s->next == NULL);
	s->check = check;
	s->next = top;
}

const char* namefor(htmlNode* n) {
	return n->name;
}

htmlNode* find_next(struct Selector* pos) {
#define POS pos->data[pos->n-1]
	assert(pos->next);
	htmlNode* cur = pos->next;
	pos->next = NULL; // dunno it anymore
	bool right(void) {
		if(!cur->next) return false;
		cur = cur->next;
		return true;
	}

	bool down(void) {
		if(!cur->children) return false;
		cur = cur->children;
		return true;
	}

	bool up(void) {
		if(!cur->parent) return false;
		fprintf(stderr,"UP %s->%s\n",
						namefor(cur),
						namefor(cur->parent));
		cur = cur->parent;
		return true;
	}
		
	for(;;) {
		// can't return yet, have to go to the next one.
		htmlNode* last = cur;
		switch(pos->last) {
		case UP:
			if(right()) pos->last = RIGHT;
			else if(up()) pos->last = UP;
			else {
				// we're done
				find_destroy(pos);
				return NULL;
			}
			break;
		case DOWN:
		case RIGHT:
			if(down()) pos->last = DOWN;
			else if(right()) pos->last = RIGHT;
			else if(up()) pos->last = UP;
			else error(23,0,"couldn't move??");
		};
		if(last->type == XML_ELEMENT_NODE && strcasecmp(last->name, pos->check)==0) {
			fprintf(stderr,"found it!\n");
			pos->next = cur;
			return last;
		}
	}
}
