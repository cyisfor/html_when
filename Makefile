CFLAGS+=-ggdb $(shell xml2-config --cflags)
LDLIBS+=$(shell xml2-config --libs)

test: source/app.c libhtmlwhen.a
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

libhtmlwhen.a: o/html_when.o o/selectors.o | o
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

o/%.o: source/%.c | o
	gcc $(CFLAGS) -c -o $@ $^
