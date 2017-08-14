#define _GNU_SOURCE // O_PATH
#include "libxmlfixes.h"
#include "ensure.h"
#include "html_when.h"
#include "cleanup.h"

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <sys/mman.h> // mmap
#include <sys/stat.h>

#include <fcntl.h> // open, O_*
#include <string.h>

#include <assert.h>
#include <dirent.h>
#include <unistd.h> // close
#include <errno.h> // ENOENT

DECLARE_CLEANUP(closedir,DIR*);
DECLARE_CLEANUP(close,int);
DECLARE_CLEANUP(xmlFreeDoc,xmlDoc*);
#define CONCAT2(a,b) a ## b
#define CONCAT(a,b) CONCAT2(a,b)
// special: have to define an inline function named unmap after every info stat struct
#define UNMAP(info,type) void CONCAT(unmap,__LINE__)(type* mem) { \
		int res = munmap(*mem,info.st_size);							\
		assert(res == 0);																\
	}																									\
	__attribute__((__cleanup__(CONCAT(unmap,__LINE__)))) type


int main(int argc, char**argv) {
	ensure0(chdir("tests"));
	cleanup(closedir) DIR* d = opendir(".");
	assert(d);
	cleanup(close) int expected = open("results",O_DIRECTORY|O_PATH);
	if(expected < 0) {
		if(errno == ENOENT) {
			int res = mkdir("results",0755);
			assert(res == 0);
			expected = open("results",O_DIRECTORY|O_PATH);
		} else {
			perror("fail");
			abort();
		}
	}
	assert(expected >= 0);
	cleanup(close) int env = open("env",O_DIRECTORY|O_PATH);
	assert(env >= 0);
	struct dirent* ent;

	void on_error(void * userData, xmlErrorPtr error) {
		if(html_when_handled_error(error)) return;
		WARN("xml error %s",error->message);
	}
	xmlSetStructuredErrorFunc(NULL,on_error);

	while(ent = readdir(d)) {
		size_t len = strlen(ent->d_name);
		if(len < 5) continue;
		if(0!=strcmp(ent->d_name+len-5,".html")) continue;
		char* name = alloca(len-4);
		memcpy(name,ent->d_name,len-5);
		name[len-5] = '\0';
		fputs(name,stdout);
		fputs("...",stdout);
		fflush(stdout);

		{
			cleanup(close) int e = openat(env,name,O_RDONLY);
			struct stat info;
			if(e >= 0) {
				assert(0==fstat(e,&info));
				UNMAP(info,char*) mem = mmap(NULL,info.st_size,PROT_READ,MAP_PRIVATE,e,0);
				assert(mem != MAP_FAILED);
				char* start = mem;
				for(;;) {
					char* eq = memchr(start,'=',info.st_size-(start-mem));
					if(eq == NULL) {
						if(start != mem + info.st_size) {
							WARN("trailing data... %.*s",info.st_size-(start-mem), start);
						}
						break;
					}
					size_t nlen = eq-start;
					++eq;
					char* end = memchr(eq,'\n',info.st_size-(eq-mem));
					size_t vlen;
					if(end == NULL) {
						// no trailing newline
						vlen = info.st_size-(eq-mem);
					} else {
						vlen = end-eq;
					}
					INFO("set env %.*s %.*s",nlen,start,vlen,eq);
					char* name = alloca(nlen+1);
					char* value = alloca(vlen+1);
					memcpy(name,start,nlen);
					memcpy(value,eq,vlen);
					start = end+1;
					if(start >= mem + info.st_size) break;
					name[nlen] = '\0';
					value[vlen] = '\0';
					setenv(name,value,1);
				}
			}
		}
		
		xmlChar* test = NULL;
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
			close(efd);
			ensure0(renameat(expected,".temp",expected,ent->d_name));
			puts("created.");
			continue;
		}
		assert(0==fstat(efd,&info));
		
		ensure_eq(tlen,info.st_size);
		UNMAP(info,xmlChar*) expected = mmap(NULL,info.st_size,PROT_READ,MAP_PRIVATE,efd,0);
		assert(expected != MAP_FAILED);
		ensure_eq(0,memcmp(test,expected,tlen));
		puts(" passed");
	}
}
