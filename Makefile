include coolmake/main.mk
coolmake/main.mk: | coolmake
	@echo Coolmake...
	[[ -e $@ ]] || exit 3
	$(S)$(MAKE)
coolmake: libxmlfixes/coolmake
	ln -rs $< $@

libxmlfixes/coolmake: | libxmlfixes
	$(MAKE) -C $|

libxmlfixes:
	sh setup.sh
