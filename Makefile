include coolmake/main.mk
# symlink dep on include is okay, b/c won't rebuild anything
coolmake/main.mk: | coolmake
	@echo Coolmake...
	@echo Coolmake...
	@if [[ -z "$(CLEANING)" ]]; then \
		[[ -e $@ ]] || exit 3; \
		$(MAKE) $(MAKECMDGOALS); \
	fi

.PRECIOUS: coolmake/main.mk
coolmake: libxmlfixes/coolmake
	ln -rs $< $@

libxmlfixes/coolmake: | libxmlfixes
	$(MAKE) -C $|

libxmlfixes:
	sh setup.sh
