LIB_TAR = bluez-5.44
LIB_PATH = $(PWD)/../libtar/bluez-5.44.tar.xz

all:
	cd $(LIB_TAR) && ./configure --prefix=$(OUT_DIR) --host=arm-linux CC=$(LIB_CC) CFLAGS="-I$(OUT_DIR)/include/ -I$(OUT_DIR)/include/glib-2.0 -I$(OUT_DIR)/include/dbus-1.0" LDFLAGS="-lncurses  -lreadline -ldbus-1 -lglib-2.0 -lical -L/$(OUT_DIR)/lib -L$(OUT_DIR)/lib" DBUS_CFLAGS="-I$(OUT_DIR)/lib/dbus-1.0/include" DBUS_LIBS="-L$(OUT_DIR)/lib" GLIB_CFLAGS="-I$(OUT_DIR)/lib/glib-2.0/include" GLIB_LIBS="-L$(OUT_DIR)/lib" ICAL_CFLAGS="-I$(OUT_DIR)/include" ICAL_LIBS="-L$(OUT_DIR)/lib" --enable-library --sysconfdir=/etc --localstatedir=/var --enable-experimental --with-systemdsystemunitdir=/lib/systemd/system --with-systemduserunitdir=/usr/lib/system --with-dbusconfdir=$(OUT_DIR)/dbus-1/system.d --enable-testing --enable-experimental  --enable-deprecated --disable-systemd --disable-udev --disable-cups --disable-obex --with-dbussystembusdir=$(OUT_DIR)/bin --with-dbussessionbusdir=$(OUT_DIR)/bin
	cd $(LIB_TAR) && make && make install 

prapre:
	rm -rf $(LIB_TAR)
	tar -xvJf $(LIB_PATH)  -C ./

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)