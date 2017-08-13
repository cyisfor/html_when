#include "html_when.h"
#include "selectors.h"
#include <libxml/HTMLtree.h>
#include <string.h> // strlen
#include <stdlib.h> // getenv
#include <stdio.h> // debugging
#include <ctype.h> // debugging
#include <assert.h>
#include <stdbool.h>

static xmlNode* found_when(xmlNode* cur) {
	bool condition = false; // <when nonexistentvar> => else clause
	const char* envval = NULL;
	xmlAttr* a = cur->properties;
	for(;a;a = a->next) {
		if(0==strcasecmp(a->name,"not")) {
			condition = !condition;
		} else if(a->name) {
			const char* name = a->name;
			while(name[0] == 'u' && name[1] == 'n') {
				name += 2;
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
				//htmlNodeDumpFileFormat(stderr,root->doc,root,"UTF-8",1);
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
			xmlNode* next = kid->next;
			xmlAddPrevSibling(cur,kid);
			kid = next;
		}
	} else {
		// negative when clause
		// remove nodes, until else, then move to parent
		for(kid=cur->children;kid;) {
			bool iselse = kid->type == XML_ELEMENT_NODE && (strcasecmp(kid->name,"else")==0);
			// remove until else
			xmlNode* next = kid->next;
			xmlUnlinkNode(kid);
			xmlFreeNode(kid);
			kid = next;

			if(iselse) {
				//  move the rest to parent, checking for val
				break;
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
	// don't check prev, since that's already been checked
	// (it either was below us, or behind us)
	xmlNode* backtrack = cur->next;
	if(!backtrack) backtrack = cur->parent;
	xmlUnlinkNode(cur);
	xmlFreeNode(cur);
	return backtrack;	
}

void html_when(xmlNode* root) {
	if(!root) return;
	switch(root->type) {
	case XML_ELEMENT_NODE:
		// breadth first so not reparsing when add to parent
		if((0==strcmp(root->name,"when"))) {
			/* start over from new root, which may get children of it, and will continue on next */
			return html_when(found_when(root));
		}
	case XML_DOCUMENT_NODE:
	case XML_HTML_DOCUMENT_NODE:
		html_when(root->children);
	};
	return html_when(root->next);
}

bool html_when_handled_error(xmlErrorPtr error) {
	if(error->code != XML_HTML_UNKNOWN_TAG) return false;
	const xmlChar* name = error->str1;
	size_t len = strlen(name);
	if(len == 3) {
		return 0 == memcmp(name,"val",3);
	}
	if(len != 4) return false;
	if(0==memcmp(name,"when",4)) return true;
	if(0==memcmp(name,"else",4)) return true;
	return false;
}
