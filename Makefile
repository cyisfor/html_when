include head.mk

CFLAGS+=-ggdb
CFLAGS+=-Ilibxml2/include/ -Inote/ -Ilibxmlfixes/src
XMLVERSION:=include/libxml/xmlversion.h

LDLIBS+=$(shell xml2-config --libs | sed -e's/-xml2//g')

LIBTOOL:=libtool --tag=CC --mode=
LINK=@echo LINK $*; $(LIBTOOL)link $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
COMPILE=@echo COMPILE $*; $(LIBTOOL)compile $(CC) $(CFLAGS) -c -o $@ $<

O=$(patsubst %,o/%.o,$N) o/note/note.o
OO=$O $L
L=libxml2/libxml2.la libhtmlwhen.la libxmlfixes/libxmlfixes.la
all: example test

N=app
example: $(OO) $(L)
	$(LINK)

libxmlfixes/libxmlfixes.la:
	$(MAKE) -C $(dir $@) $(notdir $@)

N=test
test: $(OO)
	$(LINK)

define AUTOMAKE_SUBPROJECT
$1/$2.la: $1/Makefile
	$$(MAKE) -C $1

$1/Makefile: $1/configure
	sh config-my-$1.sh

$1/configure: $1/configure.ac
	cd $1 && sh autogen.sh --help
endef

$(eval $(call AUTOMAKE_SUBPROJECT,libxml2,libxml2))

libxml2/configure.ac: libxml2 libxmlfixes/libxml2

libxml2:
	sh setup.sh

libxmlfixes/libxml2:
	$(MAKE) -C $(dir $@) setup

N=html_when selectors
libhtmlwhen.la: $(O)
	$(LINK)

%.la: o/%.o
	$(LINK)

o:
	mkdir $@

o/%.o: src/%.c libxml2/$(XMLVERSION) | o
	$(COMPILE)


o/%.o: %.c | o
	mkdir -p `dirname $@`
	$(COMPILE)

libxml2/$(XMLVERSION): libxml2/libxml2.la

setup: ./setup.sh
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
