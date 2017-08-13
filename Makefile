CFLAGS+=-ggdb -Ilibxml2/include/ -Inote/
LDLIBS+=$(shell xml2-config --libs | sed -e's/-xml2//g')
LINK=$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
O=$(patsubst %,o/%.o,$N)

all: example test

N=app
example: $O libhtmlwhen.a libxml2/.libs/libxml2.a
	$(LINK)

N=test note/note
test: $O libhtmlwhen.a libxml2/.libs/libxml2.a
	$(LINK)

define AUTOMAKE_SUBPROJECT
$1/.libs/$2.a: $1/Makefile
	cd $1 && make

$1/Makefile: $1/configure
	cd $1 && ./configure

$1/configure: $1/configure.ac
	cd $1 && sh autogen.sh --help
endef

$(eval $(call AUTOMAKE_SUBPROJECT, libxml2, libxml2))

N=libxmlfixes html_when selectors
libhtmlwhen.a: $O
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

COMPILE=$(CC) $(CFLAGS) -c -o $@ $^

o/%.o: src/%.c | o
	$(COMPILE)


o/%.o: %.c | o
	mkdir -p `dirname $@`
	$(COMPILE)
