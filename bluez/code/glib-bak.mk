LIB_TAR = glib-2.40.0
LIB_PATH = $(PWD)/../libtar/glib-2.40.0.tar.xz

all:
	cd $(LIB_TAR) && ./configure --prefix=$(OUT_DIR)  --host=arm-linux CC=$(LIB_CC) PKG_CONFIG_PATH=$(OUT_DIR)/lib/pkgconfig  glib_cv_stack_grows=no glib_cv_uscore=yes ac_cv_func_posix_getpwuid_r=yes ac_cv_func_posix_getgrgid_r=yes CPPFLAGS="-I$(OUT_DIR)/include/" LDFLAGS="-L$(OUT_DIR)/lib/"
	cd $(LIB_TAR) && make && make install

prapre:
	rm -rf $(LIB_TAR)
	tar -xvJf $(LIB_PATH)  -C ./
	cd $(LIB_TAR) && git apply $(PWD)/patch/glib/001-glib-gdate-suppress-string-format-literal-warning.patch

clean:
	cd $(LIB_TAR) && make clean

distclean:
	rm -rf $(LIB_TAR)