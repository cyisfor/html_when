CFLAGS+=-ggdb -Igumbo-parser/src
LDLIBS+=gumbo-parser/.libs/libgumbo.a

test: source/app.c source/output.c libhtmlwhen.a
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

libhtmlwhen.a: o/html_when.o o/selectors.o | o
	$(AR) $(ARFLAGS) $@ $^

o:
	mkdir $@

o/%.o: source/%.c | o
	gcc $(CFLAGS) -c -o $@ $^
