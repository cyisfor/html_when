#define _GNU_SOURCE // O_PATH
#include "libxmlfixes.h"
#include "ensure.h"
#include "html_when.h"
#include "cleanup.h"

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <assert.h>

DECLARE_CLEANUP(closedir,DIR*);

int main(int argc, char**argv) {
	ensure0(chdir("tests"));
	cleanup(closedir) DIR* d = opendir(".");
	assert(d);
	cleanup(close) int expected = open("results",O_DIR|O_PATH);
	assert(expected >= 0);
	struct dirent* ent;
	while(ent = readdir(d)) {
		size_t len = strlen(ent->d_name);
		if(len < 5) continue;
		if(0!=strcmp(ent->d_name+len-5,".html")) continue;
		fputs(end->d_name,stdout);
		fputs("...",stdout);
		fflush(stdout);
		char* test = NULL;
		int tlen;
		{
			cleanup(xmlFreeDoc) xmlDoc* doc = ({
					cleanup(close) int fd = open(ent->d_name,O_RDONLY);
					ensure_gt(fd,0);
					readFunky(fd, NULL, 0);
				});
			assert(doc);
			HTML5_plz(doc);
			html_when((xmlNode*)doc); // magic...

			htmlDocDumpMemory(doc,&test,&tlen);
		}

		cleanup(close) int efd = openat(expected,ent->d_name,O_RDONLY);
		struct stat info;
		if(efd < 0) {
			puts("expected not found. ^C to not create");
			if(getchar() == 'n') {
				continue;
			}
			efd = openat(expected,".temp",O_WRONLY|O_CREAT|O_TRUNC,0644);
			assert(efd > 0);
			size_t amt = write(efd,test,tlen);
			ensure_eq(amt, tlen);
			puts("created.");
			continue;
		}
		assert(0==fstat(efd,&info));

		ensure_eq(tlen,info.st_size);
		char* expected = mmap(NULL,info.st_size,PROT_READ,MAP_PRIVATE,efd,0);
		assert(expected != MAP_PRIVATE);
		ensure_eq(0,memcmp(test,expected,tlen));
		puts("passed");
	}
}
