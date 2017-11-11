CFLAGS+=-ggdb
CFLAGS+=-Ilibxml2/include/ -Inote/ -Ilibxmlfixes/src

o/note/note.o: o/note/

VPATH+=libxmlfixes/src libxmlfixes/
L=libxml2.la libhtmlwhen.la libxmlfixes.la

$(call AUTOMAKE_SUBPROJECT,libxml2,libxml2)

all: example test

N=app note/note
example: $(O) $(L)
	$(LINK)

include libxmlfixes/main.mk

N=test note/note
test: $(O) $(L)
	$(LINK)

libxml2/configure.ac: libxml2 libxmlfixes/libxml2

libxml2:
	sh setup.sh

libxmlfixes/libxml2:
	$(MAKE) -C $(dir $@) setup

N=html_when selectors
libhtmlwhen.la: $(O)
	$(LINK)

o/note: | o
	mkdir $@

setup: ./setup.sh
	. ./setup.sh

git-tools/pushcreate: git-tools/pushcreate.c
	$(MAKE) -C git-tools

push: setup ./git-tools/pushcreate 
	[[ -n "$(remote)" ]]
	./git-tools/pushcreate "$(remote)"
	(cd libxml2 && ../git-tools/pushcreate "$(remote)/libxml2")
