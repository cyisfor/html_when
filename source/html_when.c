#include "html_when.h"
#include "selectors.h"
#include <string.h> // strlen
#include <stdlib.h> // getenv
#include <stdio.h> // debugging
#include <ctype.h> // debugging
#include <assert.h>

void html_when(xmlNode* root) {
	if(!root) return;

	struct Selector selector = {};
	find_start(&selector, root, "when", NULL);
	for(;;) {
		xmlNode* cur = find_next(&selector);
		if(!cur) return;

		bool condition = false; // <when nonexistentvar> => else clause
		const char* envval = NULL;
		xmlAttr* a = cur->properties;
		for(;a;a = a->next) {
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
					if(!*a->children) {
						if(envval != NULL) {
							// without a value, no limitation to making falsity true and vice versa
 							condition = !condition;
						}
					} else {
						// a->value is already unescaped
						if(envval && 0==strcasecmp(a->children,envval)) {
							condition = !condition;
						}
					}
				}
			}
		}

		size_t envvallen = envval ? strlen(envval) : 0;

		xmlNode* replaceval(xmlNode* n) {
			if(!envval) return n;
			if(0 == strcasecmp(n->name,"val")) {
				xmlNode* new = xmlNewText(envval); // need make 1 per replacement
				xmlReplaceNode(n,new);
				n = new;
			} else if(kid->type == XML_ELEMENT_NODE) {
				xmlNode* kid = n->children;
				for(;kid;kid = kid->next) {
					kid = replaceval(kid);
				}
			}
			return n;
		}
		
		xmlNode* kid = cur->children;
		if(condition) {
			// positive when clause
			// move children to prev-siblings (to maintain order) until else
			// then drop the rest
			while(kid) {
				if(kid->type == XML_ELEMENT_NODE) {
					if(strcasecmp(kid->name,"else")==0) {
						// remove this, and all the rest after.
						kid->prev = NULL;
						xmlFreeNodeList(kid);
						kid = NULL;
						break;
					} else {
						if(envval) {
							kid = replaceval(kid);
						}
					}
				}
				xmlAddPrevSibling(cur,kid);
				kid = kid->next;
			}
		} else {
			// negative when clause
			// remove nodes, until else, then move to parent
			for(kid=cur->children;kid;kid = kid->next) {
				if(kid->type == XML_ELEMENT_NODE) {
					if(strcasecmp(kid->name,"else")==0) {
						// remove this, move the rest to parent, checking for val
						xmlUnlinkNode(kid);
						xmlFreeNode(kid);
						kid = NULL;
						break;
					}
				}
			}
			while(kid) {
				kid = replaceval(kid);
				xmlNode* next = kid->next;
				xmlAddPrevSibling(cur,kid);
				kid = next;
			}
		}
		// cur should be empty now
		xmlUnlinkNode(cur);
		xmlFreeNode(cur);
	}
}
