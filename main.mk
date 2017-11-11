CFLAGS+=-ggdb
CFLAGS+=-Ilibxml2/include/ -Inote/ -Ilibxmlfixes/src

include coolmake/top.mk

o/note/note.o: o/note/

LDLIBS+=libxml2/libxml2.la libxmlfixes/libxmlfixes.la

$(call AUTOMAKE_SUBPROJECT,libxml2,libxml2)

all: example test

N=app note/note
NN=libhtmlwhen.la
OUT=example
$(eval $(PROGRAM))

$(OBJECTS): | libxml2

N=test note/note
OUT=test
$(eval $(PROGRAM))

libxml2/configure.ac: libxml2 libxmlfixes/libxml2

libxml2:
	sh setup.sh

libxmlfixes/libxml2:
	$(MAKE) -C $(dir $@) setup

N=html_when selectors
OUT=libhtmlwhen.la
$(eval $(PROGRAM))

$(O)/note: | $(O)
	mkdir $@

$(O)/note/note.lo: | $(O)/note
	$(COMPILE)

setup: ./setup.sh
	. ./setup.sh

git-tools/pushcreate: git-tools/pushcreate.c
	$(MAKE) -C git-tools

push: setup ./git-tools/pushcreate 
	[[ -n "$(remote)" ]]
	./git-tools/pushcreate "$(remote)"
	(cd libxml2 && ../git-tools/pushcreate "$(remote)/libxml2")


$(eval $(PUSHVARS))
include libxmlfixes/main.mk
$(eval $(POPVARS))
