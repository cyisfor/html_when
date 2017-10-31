include coolmake/head.mk

CFLAGS+=-ggdb
CFLAGS+=-Ilibxml2/include/ -Inote/ -Ilibxmlfixes/src

o/note/note.o: o/note/

L=libxml2/libxml2.la libhtmlwhen.la libxmlfixes/libxmlfixes.la

all: example test

N=app note/note
example: $(O) $(L)
	$(LINK)

libxmlfixes/libxmlfixes.la:
	$(MAKE) -C $(dir $@) $(notdir $@)

N=test note/note
test: $(O) $(L)
	$(LINK)

$(call AUTOMAKE_SUBPROJECT,libxml2,libxml2)

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

coolmake/head.mk coolmake/tail.mk setup: ./setup.sh
	. ./setup.sh

./setup.sh: git-tools/funcs.sh
git-tools/funcs.sh:
	git submodule update --init

git-tools/pushcreate: git-tools/pushcreate.c
	$(MAKE) -C git-tools

push: setup ./git-tools/pushcreate 
	[[ -n "$(remote)" ]]
	./git-tools/pushcreate "$(remote)"
	(cd libxml2 && ../git-tools/pushcreate "$(remote)/libxml2")

include coolmake/tail.mk
