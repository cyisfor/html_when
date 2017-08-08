CFLAGS+=-ggdb -Ilibxml2/include/
LDLIBS+=libxml2/.libs/libxml2.a

test: source/app.c libhtmlwhen.a
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

libhtmlwhen.a: o/html_when.o o/selectors.o | o
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

o/%.o: source/%.c | o
	gcc $(CFLAGS) -c -o $@ $^
