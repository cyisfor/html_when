#include "selectors.h"
#include <stdlib.h> // malloc...
#include <error.h>
#include <assert.h>
#include <stdio.h>
static void find_destroy(struct Selector* s) {
	free(s->data);
	s->data = NULL; // just in case
	s->n = 0;
}

void find_start(struct Selector* s, GumboNode* top, GumboTag check, void* udata) {
	assert(s->last == DOWN);
	assert(s->next == NULL);
	s->check = check;
	if(udata) s->udata = udata;
	if(top) s->next = top;
	s->n = 0;
}

const char* namefor(GumboNode* n) {
	switch(n->type) {
	case GUMBO_NODE_ELEMENT:
	case GUMBO_NODE_TEMPLATE:
	case GUMBO_NODE_DOCUMENT:
		return gumbo_normalized_tagname(n->v.element.tag);
	case GUMBO_NODE_TEXT:
		return "(text)";
	default:
		return "ehunno";
	}
}


GumboNode* find_next(struct Selector* pos) {
#define POS pos->data[pos->n-1]
	assert(pos->next);
	GumboNode* cur = pos->next;
	GumboNode* parent = cur->parent;
	pos->next = NULL; // dunno it anymore
	bool right(void) {
		if(!parent) {
			// there are no siblings
			return false;
		}
		if(POS + 1 >= parent->v.element.children.length) return false;
		GumboNode* next = parent->v.element.children.data[++POS];
		fprintf(stderr,"RIGHT %d %d %s->%s\n",POS+1,
						parent->v.element.children.length,
						namefor(cur),
						namefor(next));
		assert(next != NULL);
		cur = next;
		return true;
	}
	bool down(void) {
		if(cur->v.element.children.length == 0) return false;
		if(pos->space <= pos->n) {
			pos->space += 0x400;
			pos->data = realloc(pos->data, pos->space);
		}
		pos->data[++pos->n] = 0; // always start on the left
		GumboNode* next = cur->v.element.children.data[0];
		fprintf(stderr,"DOWN %d %d %s->%s\n",pos->n,
						cur->v.element.children.length,
						namefor(cur),
						namefor(next));
		parent = cur;
		cur = next;
		return true;
	}

	bool up(void) {
		if(!parent) return false;
		fprintf(stderr,"UP %d %d %s->%s\n",pos->n,
						parent->v.element.children.length,
						namefor(cur),
						namefor(parent));
		--pos->n;
		cur = parent;
		parent = parent->parent;
		return true;
	}
		
	for(;;) {
		switch(cur->type) {
		case GUMBO_NODE_TEXT:
		case GUMBO_NODE_CDATA:
		case GUMBO_NODE_COMMENT:
		case GUMBO_NODE_WHITESPACE:
/*			it's not a tag
				if(cur->tag == pos->check) {
				pos->cur = cur;
				return cur;
			} */
			// we need to NOT go down from here
			pos->last = UP;
			break;
		case GUMBO_NODE_DOCUMENT:
		case GUMBO_NODE_ELEMENT:
		case GUMBO_NODE_TEMPLATE:
			// it's a tag, go to the end
			break;
		default:
			error(0,0,"wat is this");
			abort();
		};
		// can't return yet, have to go to the next one.
		GumboNode* last = cur;
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
		if(last->v.element.tag == pos->check) {
			fprintf(stderr,"found it! %p\n",cur->parent);
			pos->next = cur;
			return last;
		}
	}
}
