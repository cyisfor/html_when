#include "ensure.h"
#include "html_when.h"
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>


int main(int argc, char**argv) {
	htmlDoc* doc = htmlReadFd(0,"","UTF-8",
														HTML_PARSE_RECOVER |
														HTML_PARSE_NONET |
														HTML_PARSE_COMPACT);
	ensure_ne(NULL,doc)
	html_when(doc->children);
	htmlSaveFile("/tmp/output.deleteme",doc);
}
