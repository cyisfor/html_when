CFLAGS+=-ggdb
CFLAGS+=-Ilibxml2/include/ -Inote/ -Ilibxmlfixes/src

include coolmake/top.mk

$(call AUTOMAKE_SUBPROJECT,libxml2,libxml2)

all: example test

LIBS=libhtmlwhen.la libxml2/libxml2.la libxmlfixes/libxmlfixes.la

N=app note/note
OUT=example
$(eval $(PROGRAM))

$(OBJECTS): | libxml2

N=test note/note
OUT=test
$(eval $(PROGRAM))

libxml2: | libxmlfixes/libxml2
	$(SYMLINK)

libxmlfixes/libxml2: | libxmlfixes
	$(MAKE) -C $| libxml2

LIBS=libxml2/libxml2.la libxmlfixes/libxmlfixes.la
N=html_when selectors
OUT=libhtmlwhen.la
$(eval $(PROGRAM))

undefine LIBS

$(O)/note: | $(O)
	mkdir $@

$(O)/note/note.lo: note/note.c | $(O)/note
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
