CFLAGS+=-ggdb
CFLAGS+=-Ilibxml2/include/ -Inote/ -Ilibxmlfixes/src

include coolmake/top.mk

o/note/note.o: o/note/

VPATH+=libxmlfixes/src libxmlfixes/
LDLIBS+=libxml2.la libhtmlwhen.la libxmlfixes.la

$(call AUTOMAKE_SUBPROJECT,libxml2,libxml2)

all: example test

N=app note/note
OUT=example
$(eval $(PROGRAM))

$(eval $(PUSHVARS))
include libxmlfixes/main.mk
$(eval $(POPVARS))

N=test note/note
OUT=test
$(eval $(PROGRAM))

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
