CFLAGS+=-ggdb -O2 -Ilibxml2/include/ -Inote/

XMLVERSION:=include/libxml/xmlversion.h

LDLIBS+=$(shell xml2-config --libs | sed -e's/-xml2//g')
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

N=libxmlfixes html_when selectors
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

setup:
	. ./setup.sh 


push: setup
	[[ -n "$$remote" ]]
	git push libxml2 $$remote/libxml2
