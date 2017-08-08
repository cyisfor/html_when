#include "ensure.h"
#include "html_when.h"
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>


int main(int argc, char**argv) {
	htmlParserCtxt* ctxt;
	xmlParserInputBufferPtr input;
	xmlParserInputPtr stream;

	xmlInitParser();
	input = xmlParserInputBufferCreateFd(0, XML_CHAR_ENCODING_UTF8);
	assert (input != NULL);
	ctxt = htmlNewParserCtxt();
	assert (ctxt != NULL);
	ctxt->recovery = 1;
	void	on_error(void * userData, xmlErrorPtr error) {
		return;
	}
	ctxt->sax->serror = &on_error;

	xmlDoc* doc = htmlCtxtReadFd(ctxt,
															 0,"","UTF-8",
															 HTML_PARSE_RECOVER |
															 HTML_PARSE_NONET |
															 HTML_PARSE_COMPACT);
	ensure_ne(NULL,doc)
	html_when(doc->children);
	htmlSaveFile("/tmp/output.deleteme",doc);
}
