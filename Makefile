include coolmake/main.mk
coolmake/main.mk: | coolmake
	$(MAKE)
coolmake: libxmlfixes/coolmake
	ln -rs $< $@

libxmlfixes/coolmake: | libxmlfixes
	$(MAKE) -C $|

libxmlfixes:
	sh setup.sh
