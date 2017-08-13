CFLAGS+=-ggdb -Ilibxml2/include/
LDLIBS+=$(shell xml2-config --libs | sed -e's/-xml2//g')
LINK=$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
O=$(patsubst %,o/%.o,$N)

N=app.c
example: $O libhtmlwhen.a libxml2/.libs/libxml2.a
	$(LINK)

libxml2/.libs/libxml2.a: libxml2/Makefile
	cd libxml2 && make

libxml2/Makefile: libxml2/configure
	cd libxml2 && ./configure

libxml2/configure: libxml2/configure.ac
	cd libxml2 && sh autogen.sh --help

N=libxmlfixes html_when selectors
libhtmlwhen.a: $O
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

o/%.o: source/%.c | o
	gcc $(CFLAGS) -c -o $@ $^
