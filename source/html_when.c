#include "html_when.h"
#include "selectors.h"
#include <libxml/HTMLtree.h>
#include <string.h> // strlen
#include <stdlib.h> // getenv
#include <stdio.h> // debugging
#include <ctype.h> // debugging
#include <assert.h>
#include <stdbool.h>

void html_when(xmlNode* root) {
	if(!root) return;

	struct Selector selector = {};
	find_start(&selector, "when");
	xmlNode* cur;
	for(cur = find_next(root, &selector);
			cur;
			cur = find_next(cur, &selector)) {
		htmlNodeDumpFileFormat(stderr,root->doc,root,"UTF-8",1);
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
					if(envval != NULL) {
						assert(a->type == XML_ATTRIBUTE_NODE);
						bool noval = a->children == NULL;
						if(noval) {
							// without a value, no limitation to making falsity true and vice versa
 							condition = !condition;
						} else {
							assert(a->children->type == XML_TEXT_NODE);
							assert(a->children->next == NULL);
							xmlChar* val = a->children->content;
							// val is already unescaped
							if(envval && 0==strcasecmp(val,envval)) {
								condition = !condition;
							}
						}
					}
				}
			}
		}

		xmlNode* replaceval(xmlNode* n) {
			if(!envval) return n;
			if(n->type == XML_ELEMENT_NODE) {
				if(0 == strcasecmp(n->name,"val")) {
					xmlNode* new = xmlNewText(envval); // need make 1 per replacement
					xmlReplaceNode(n,new);
					return new;
				} else if(0 == strcasecmp(n->name,"when")) {
					// should be impossible? we handle from bottom up!
					perror("what?");
					htmlNodeDumpFileFormat(stderr,root->doc,root,"UTF-8",1);
					abort();
				} else {
					xmlNode* kid = n->children;
					for(;kid;kid = kid->next) {
						kid = replaceval(kid);
					}
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
						// ignore this, and all the rest after.
						// will be removed with cur
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
			for(kid=cur->children;kid;) {
				if(kid->type == XML_ELEMENT_NODE) {
					if(strcasecmp(kid->name,"else")==0) {
						// remove this, move the rest to parent, checking for val
						xmlNode* next = kid->next;
						xmlUnlinkNode(kid);
						xmlFreeNode(kid);
						kid = next;
						break;
					}
				}
				// remove until else
				xmlNode* next = kid->next;
				xmlUnlinkNode(kid);
				xmlFreeNode(kid);
				kid = next;
			}
			while(kid) {
				kid = replaceval(kid);
				xmlNode* next = kid->next;
				xmlAddPrevSibling(cur,kid);
				kid = next;
			}
		}
		// cur should be empty now
		xmlNode* backtrack = cur->prev;
		if(!backtrack) backtrack = cur->next;
		if(!backtrack) backtrack = cur->parent;
		xmlUnlinkNode(cur);
		xmlFreeNode(cur);
		cur = backtrack;
	}
}
