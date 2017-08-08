#include "html_when.h"
#include "selectors.h"
#include <string.h> // strlen
#include <stdlib.h> // getenv
#include <stdio.h> // debugging
#include <ctype.h> // debugging
#include <assert.h>

// gumbo sucks
void gumbo_destroy_node(const GumboOptions* options, GumboNode* node);

static void movenodes(GumboNode* destnode, int inspoint, void** src, int numkids) {
	/* intended to replace a node with (some sequence of) its children so
		 a b c d e f
		 d: da db dc
		 => a b c da db e f
		 so first make enough space
		 a b c d e f 0
		 then copy all but doomed, knowing doomed is a+3
		 inspoint = 3
		 src = 3+1
		 amt = numkids-1
		 dest = start+numkids (NOT -1)
		 newlen = dest+amt
		 note: if dest-src > amt, no overlap can memcpy
		 memmove should do this but it doesn't.
		 a b c d e e f
		 then copy children in
		 a b c da db e f
		 then destroy dc
		 then set d children to length 0 and destroy d
	*/
	GumboVector* dest = &destnode->v.element.children;
	if(numkids == 0) {
		return;
	} else if(numkids == 1) {
		// if it's 1 kid, we can just replace the <when> with the kid
		dest->data[inspoint] = src[0];
		((GumboNode*)src[0])->parent = destnode;
		return;
	} 
	int srcpoint = inspoint+1;
	int amt = numkids - 1; // skipping the current doomed one
		
	int destpoint = srcpoint+numkids;
	int newlen = dest->length + amt;
	if(dest->capacity < newlen) {
		dest->capacity = ((newlen >> 10)+1)<<10;
		// just using malloc, so this... works
		dest->data = realloc(dest->data,dest->capacity);
	}
	dest->length = newlen;
	// copy the old kids out of the way
	if(destpoint - srcpoint > amt) {
		// no overlap
		memcpy(dest->data+destpoint,
					 dest->data+srcpoint,
					 amt);
	} else {
		memmove(dest->data+destpoint,
						dest->data+srcpoint,
						amt);
	}
	// okay we've saved the old kids, leaving a space in the middle... now fill it up
	// with the new kids

	memcpy(dest->data+srcpoint,src,numkids);

	// be sure to set all the links
	int i;
	for(i=0;i<numkids;++i) {
		((GumboNode*)dest->data[srcpoint+i])->parent = destnode;
	}
}

const char* namefor(GumboNode* n);

void print_tree(GumboNode* root, int level) {
	int i;
	for(i=0;i<level;++i) {
		fputc('-',stderr);
	}
	switch(root->type) {
		case GUMBO_NODE_DOCUMENT:
		case GUMBO_NODE_ELEMENT:
		case GUMBO_NODE_TEMPLATE:
			// it's an element
			fputs(namefor(root),stderr);
			fputc('\n',stderr);
			{ int j;
				for(j=0;j<root->v.element.children.length;++j) {
					print_tree(root->v.element.children.data[j],level+1);
				}
			}
			break;
	case GUMBO_NODE_TEXT:
	case GUMBO_NODE_CDATA:
	case GUMBO_NODE_COMMENT:
	case GUMBO_NODE_WHITESPACE:
		{
			int len = strlen(root->v.text.text);
			if(len > 20) len = 20;
			fputs("text: ",stderr);
			{ int j;
				const char* text = root->v.text.text;
				for(j=0;j<len;++j) {
					if(text[j] == '\n') continue;
					if(isspace(text[j]))
						fputc('.',stderr);
					else
						fputc(text[j],stderr);
				}
			}
			fputc('\n',stderr);
		}

	}
}

	

void html_when(GumboNode* root) {
	if(!root) return;

	struct Selector selector = {};
	find_start(&selector, root, GUMBO_TAG_WHEN, NULL);
	for(;;) {
		GumboNode* cur = find_next(&selector);
		if(!cur) return;

		print_tree(root, 0);
		fputs("==============\n",stderr);

		bool condition = false; // <when nonexistentvar> => else clause
		const char* envval = NULL;
		int i;
		for(i=0;i<cur->v.element.attributes.length;++i) {
			GumboAttribute* a = (GumboAttribute*)cur->v.element.attributes.data[i];
			if(0==strcasecmp(a->name,"not")) {
				condition = !condition;
			} else if(a->name) {
				const char* name = a->name;
				while(*name && name[0] == '!') {
					++name;
					condition = !condition;
				}
				if(*name) {
					envval = getenv(name);
					if(!*a->value) {
						if(envval != NULL) {
							// without a value, no limitation to making falsity true and vice versa
 							condition = !condition;
						}
					} else {
						// a->value is already unescaped
						if(envval && 0==strcasecmp(a->value,envval)) {
							condition = !condition;
						}
					}
				}
			}
		}

		size_t envvallen = envval ? strlen(envval) : 0;

		void replaceval(int i, GumboNode* parent) {
			GumboNode* n = ((GumboNode*)parent->v.element.children.data[i]);
			if(n->type == GUMBO_NODE_ELEMENT) {
				if(n->v.element.tag == GUMBO_TAG_VAL) {
					GumboNode* new = calloc(1,sizeof(GumboNode));
					new->type = GUMBO_NODE_TEXT;
					new->index_within_parent = i;
					new->parse_flags = n->parse_flags;
					new->v.text.original_text.data = envval;
					new->v.text.original_text.length = envvallen;
					parent->v.element.children.data[i] = new;
					new->parent = parent;
					n->parent = NULL;
					gumbo_destroy_node(&kGumboDefaultOptions, n);
					if(selector.next == n)
						selector.next = new;
					n = new;
				} else {
					int j;
					for(j=0;j<n->v.element.children.length;++j) {
						replaceval(j,n);
					}
				}
			}
		}	
		
		GumboVector* kids = &cur->v.element.children;
		// need <inspoint> 
		// selector->data[selector->n-1] should have that 
		GumboNode* parent = cur->parent;
		int inspoint = selector.data[selector.n-1];

		int elsepoint = kids->length;
		if(condition) {
			// positive when clause
			// find else, if we can
			for(i=0;i<kids->length;++i) {
				GumboNode* kid = (GumboNode*) kids->data[i];
				if(kid->type == GUMBO_NODE_ELEMENT) {
					if(envval) {
						replaceval(i,cur);
					}
					if(kid->v.element.tag == GUMBO_TAG_ELSE) {
						// remove this, and all the rest after.
						elsepoint = i;
						break;
					}
				}
			}
			movenodes(cur->parent, inspoint, kids->data, elsepoint);
			// destroy the dangling kids...
			for(i=elsepoint;i<kids->length;++i) {
				GumboNode* kid = (GumboNode*) kids->data[i];
				assert(selector.next != kid);
				kid->parent = NULL;
				gumbo_destroy_node(&kGumboDefaultOptions, kid);
			}
		} else {
			// negative when clause
			// try to find else...
			for(i=0;i<kids->length;++i) {
				GumboNode* kid = (GumboNode*) kids->data[i];
				if(kid->type == GUMBO_NODE_ELEMENT &&
					 kid->v.element.tag == GUMBO_TAG_ELSE) {
					elsepoint = i;
					// destroy kids proactively!
					assert(i == 0 || selector.next != kid);
					kid->parent = NULL;
					gumbo_destroy_node(&kGumboDefaultOptions, kid);
					break;
				}
				assert(i == 0 || selector.next != kid);
				kid->parent = NULL;
				gumbo_destroy_node(&kGumboDefaultOptions, kid);				
			}
			// move everything past elsepoint into parent
			if(elsepoint + 1 < kids->length) {
				movenodes(cur->parent,elsepoint+1,kids->data,kids->length-elsepoint-1);
			}
			if(elsepoint >= 0) {
				//kids[0] is going to be the next selected node... destroyed
				//let's pick the first non-deleted kid... or the sibling after the when.
				selector.next = cur->parent->v.element.children.data[elsepoint];
			}
			if(envval) {
				for(i=elsepoint+1;i<kids->length;++i) {					
					replaceval(i,cur);
				}
			}
		}
		// now we should zero out kids, so destroy_node doesn't delete them
		// all kids should be moved and destroyed by now
		// move semantics
		kids->length = 0;
		// destroy <when> and point us at the first kid
		cur->parent = NULL;
		gumbo_destroy_node(&kGumboDefaultOptions, cur);
		GumboNode* next = (GumboNode*)parent->v.element.children.data[inspoint];
		if(selector.next == cur) {
			selector.next = next;
		}
	}
}
