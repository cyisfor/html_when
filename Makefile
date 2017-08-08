PKG:=libxml2
CFLAGS+=-ggdb $(shell pkg-config --cflags $(PKG))
LDLIBS+=$(shell pkg-config --libs $(PKG))

test: source/app.c source/output.c libhtmlwhen.a
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

libhtmlwhen.a: o/html_when.o o/selectors.o | o
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

o/%.o: source/%.c | o
	gcc $(CFLAGS) -c -o $@ $^
