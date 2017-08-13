#include "libxmlfixes.h"
#include "ensure.h"
#include "html_when.h"
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <assert.h>

int main(int argc, char**argv) {
	ensure0(chdir("tests"));
	DIR* d = opendir(".");
	assert(d);
	struct dirent* ent;
	while(ent = readdir(d)) {
		size_t len = strlen(ent.d_name);
		if(len < 5) continue;
		if(0!=strcmp(d_name+len-5,".html")) continue;
		
		xmlDoc* doc = htmlCtxtReadFd(ctxt,
															 0,"","UTF-8",
															 HTML_PARSE_RECOVER |
															 HTML_PARSE_NONET |
															 HTML_PARSE_COMPACT);
	ensure_ne(NULL,doc)

	HTML5_plz(doc);
	htmlNodeDumpFileFormat(stderr,doc,(xmlNode*)doc,"UTF-8",1);
	html_when((xmlNode*)doc); // magic...
	puts("now...");
	htmlNodeDumpFileFormat(stderr,doc,(xmlNode*)doc,"UTF-8",1);
	htmlSaveFileEnc("/tmp/output.deleteme",doc,"UTF-8");
}
