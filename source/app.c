#include "ensure.h"
#include "html_when.h"
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char**argv) {
	htmlParserCtxt* ctxt = htmlCreatePushParserCtxt
		(NULL,NULL,
		 LITLEN("<!DOCTYPE html>\n<html><head/><body>\n"),
		 "stdin",XML_CHAR_ENCODING_UTF8)
	ctxt->recovery = 1;
	void	on_error(void * userData, xmlErrorPtr error) {
		fprintf(stderr,"um %s %s\n",error->message,
						error->level == XML_ERR_FATAL ? "fatal..." : "ok");
		return;
	}
	xmlSetStructuredErrorFunc(NULL,on_error);
	ctxt->sax->serror = &on_error;

	struct stat info;
	assert(0==fstat(0,&info));
	char* buf = mmap(0,info.st_size,PROT_READ,MAP_PRIVATE,0,0);
	assert(buf != MAP_FAILED);

	xmlParseChunk(ctxt,buf,info.st_size,0);
	xmlParseChunk(ctxt,LITLEN("</body></html>"),1);
	xmlDoc* doc = ctxt->doc;
	ensure_ne(NULL,doc)
	html_when(doc->children);
	htmlSaveFile("/tmp/output.deleteme",doc);
}
