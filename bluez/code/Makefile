
ifneq ($(platform),)
ifeq ($(platform), dv300)
export PLATFORM=DV300
export LIB_CC=arm-himix200-linux-gcc
export OUT_DIR=/home/tianyexing/Documents/code/github_src/c_cpp/bluez/build_out
else ifeq ($(platform), x86)
PLATFORM=x86
export LIB_CC=gcc
export OUT_DIR=/home/tianyexing/Documents/code/github_src/c_cpp/bluez/build_out_x86
else
$(error not suport platform=$(platform))
endif

else
$(error Usage: make platform=dv300|x86))
endif

all:
	$(MAKE) -f ./zlib.mk
ifneq ($(platform), x86)
	$(MAKE) -f ./libffi.mk
endif
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
