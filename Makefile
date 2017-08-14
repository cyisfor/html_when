CFLAGS+=-ggdb -Ilibxml2/include/ -Inote/

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

setup: libxml2 note

define SYNC
	if [[ ! -d $1 ]]; then \
		git clone --recurse-submodules $2 pending-$1 && \
		$(MAKE) -C pending-$1 && \
		mv pending-$1 $1 ; \
	else \
		cd $1 && git pull; \
	fi
endef
.PHONY: libxml2 note
libxml2:
	$(call SYNC,$@,/extra/home/packages/git/libxml2/)
#	$(call SYNC,$@,git://git.gnome.org/libxml2)

note:
	$(call SYNC,$@,../note)
#	$(call SYNC,$@,https://github.com/cyisfor/note.git)
