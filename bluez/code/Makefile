export LIB_CC=arm-himix200-linux-gcc
export OUT_DIR=/disk4/liangshen/practice/bluez_outdir

all:
	$(MAKE) -f ./zlib.mk
	$(MAKE) -f ./libffi.mk
	$(MAKE) -f ./glib.mk
	$(MAKE) -f ./expat.mk
	$(MAKE) -f ./dbus.mk
	$(MAKE) -f ./libical.mk
	$(MAKE) -f ./ncurses.mk
	$(MAKE) -f ./readline.mk
	$(MAKE) -f ./bluez.mk

prapre:
	$(MAKE) prapre -f ./zlib.mk
	$(MAKE) prapre -f ./libffi.mk
	$(MAKE) prapre -f ./glib.mk
	$(MAKE) prapre -f ./expat.mk
	$(MAKE) prapre -f ./dbus.mk
	$(MAKE) prapre -f ./libical.mk
	$(MAKE) prapre -f ./ncurses.mk
	$(MAKE) prapre -f ./readline.mk
	$(MAKE) prapre -f ./bluez.mk

clean:
	$(MAKE) clean -f ./zlib.mk
	$(MAKE) clean -f ./libffi.mk
	$(MAKE) clean -f ./glib.mk
	$(MAKE) clean -f ./expat.mk
	$(MAKE) clean -f ./dbus.mk
	$(MAKE) clean -f ./libical.mk
	$(MAKE) clean -f ./ncurses.mk
	$(MAKE) clean -f ./readline.mk
	$(MAKE) clean -f ./bluez.mk

distclean:
	$(MAKE) distclean -f ./zlib.mk
	$(MAKE) distclean -f ./libffi.mk
	$(MAKE) distclean -f ./glib.mk
	$(MAKE) distclean -f ./expat.mk
	$(MAKE) distclean -f ./dbus.mk
	$(MAKE) distclean -f ./libical.mk
	$(MAKE) distclean -f ./ncurses.mk
	$(MAKE) distclean -f ./readline.mk
	$(MAKE) distclean -f ./bluez.mk
