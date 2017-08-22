F:=$(shell $(MAKE) -C libxmlfixes >/dev/null)

CFLAGS+=-ggdb -O2 -Ilibxml2/include/ -Inote/ -Ilibxmlfixes
XMLVERSION:=include/libxml/xmlversion.h

LDLIBS+=$(shell xml2-config --libs | sed -e's/-xml2//g')
LDLIBS+=./libxmlfixes/libxmlfixes.a
LINK=$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
O=$(patsubst %,o/%.o,$N) o/note/note.o
OO=$O $L
L=libxml2/.libs/libxml2.a libhtmlwhen.a 
all: example test

N=app
example: $(OO)
	$(LINK)

N=test
test: $(OO)
	$(LINK)

define AUTOMAKE_SUBPROJECT
$1/.libs/$2.a: $1/Makefile
	$(MAKE) -C $1

$1/Makefile: $1/configure
	cd $1 && ./configure

$1/configure: $1/configure.ac
	cd $1 && sh autogen.sh --help
endef

$(eval $(call AUTOMAKE_SUBPROJECT,libxml2,libxml2))

N=html_when selectors
libhtmlwhen.a: $(O) libxml2/.libs/libxml2.a
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

COMPILE=$(CC) $(CFLAGS) -c -o $@ $<

o/%.o: src/%.c libxml2/$(XMLVERSION) | o
	$(COMPILE)


o/%.o: %.c | o
	mkdir -p `dirname $@`
	$(COMPILE)

libxml2/$(XMLVERSION): libxml2/.libs/libxml2.a

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
