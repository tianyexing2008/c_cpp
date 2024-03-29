# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.
#
# - Modified by Owen Taylor <otaylor@redhat.com> to use GETTEXT_PACKAGE
#   instead of PACKAGE and to look for po2tbl in ./ not in intl/
#
# - Modified by jacob berkman <jacob@ximian.com> to install
#   Makefile.in.in and po2tbl.sed.in for use with glib-gettextize

GETTEXT_PACKAGE = glib20
PACKAGE = glib
VERSION = 2.40.0

SHELL = /bin/sh


srcdir = .
top_srcdir = ..


prefix = /disk4/liangshen/practice/bluez_outdir
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
libdir = ${exec_prefix}/lib
localedir = $(libdir)/locale
gnulocaledir = $(datadir)/locale
gettextsrcdir = $(datadir)/glib-2.0/gettext/po
subdir = po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = mkdir -p

CC = arm-himix200-linux-gcc
GENCAT = @GENCAT@
GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
MSGFMT_OPTS = -c
XGETTEXT = /usr/bin/xgettext
MSGMERGE = msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2
CPPFLAGS = -I/disk4/liangshen/practice/bluez_outdir/include/ -pthread

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = 
POFILES =  af.po am.po an.po ar.po as.po ast.po az.po be.po be@latin.po bg.po bn.po bn_IN.po bs.po ca.po ca@valencia.po cs.po cy.po da.po de.po dz.po el.po en_CA.po en_GB.po en@shaw.po eo.po es.po et.po eu.po fa.po fi.po fr.po ga.po gl.po gu.po he.po hi.po hr.po hu.po hy.po id.po is.po it.po ja.po ka.po kk.po kn.po ko.po ku.po lt.po lv.po mai.po mg.po mk.po ml.po mn.po mr.po ms.po nb.po nds.po ne.po nl.po nn.po oc.po or.po pa.po pl.po ps.po pt.po pt_BR.po ro.po ru.po rw.po si.po sk.po sl.po sq.po sr.po sr@latin.po sr@ije.po sv.po ta.po te.po tg.po th.po tl.po tr.po ug.po tt.po uk.po vi.po wa.po xh.po yi.po zh_CN.po zh_HK.po zh_TW.po
GMOFILES =  af.gmo am.gmo an.gmo ar.gmo as.gmo ast.gmo az.gmo be.gmo be@latin.gmo bg.gmo bn.gmo bn_IN.gmo bs.gmo ca.gmo ca@valencia.gmo cs.gmo cy.gmo da.gmo de.gmo dz.gmo el.gmo en_CA.gmo en_GB.gmo en@shaw.gmo eo.gmo es.gmo et.gmo eu.gmo fa.gmo fi.gmo fr.gmo ga.gmo gl.gmo gu.gmo he.gmo hi.gmo hr.gmo hu.gmo hy.gmo id.gmo is.gmo it.gmo ja.gmo ka.gmo kk.gmo kn.gmo ko.gmo ku.gmo lt.gmo lv.gmo mai.gmo mg.gmo mk.gmo ml.gmo mn.gmo mr.gmo ms.gmo nb.gmo nds.gmo ne.gmo nl.gmo nn.gmo oc.gmo or.gmo pa.gmo pl.gmo ps.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo rw.gmo si.gmo sk.gmo sl.gmo sq.gmo sr.gmo sr@latin.gmo sr@ije.gmo sv.gmo ta.gmo te.gmo tg.gmo th.gmo tl.gmo tr.gmo ug.gmo tt.gmo uk.gmo vi.gmo wa.gmo xh.gmo yi.gmo zh_CN.gmo zh_HK.gmo zh_TW.gmo
DISTFILES = LINGUAS ChangeLog Makefile.in.in POTFILES.in $(GETTEXT_PACKAGE).pot \
$(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
	../gio/gaction.c \
	../gio/gactiongroup.c \
	../gio/gappinfo.c \
	../gio/gapplication.c \
	../gio/gapplicationcommandline.c \
	../gio/gapplication-tool.c \
	../gio/gasynchelper.c \
	../gio/gasyncinitable.c \
	../gio/gasyncresult.c \
	../gio/gbufferedinputstream.c \
	../gio/gbufferedoutputstream.c \
	../gio/gbytesicon.c \
	../gio/gcancellable.c \
	../gio/gcharsetconverter.c \
	../gio/gcontenttype.c \
	../gio/gcontenttype-win32.c \
	../gio/gconverter.c \
	../gio/gconverterinputstream.c \
	../gio/gconverteroutputstream.c \
	../gio/gcredentials.c \
	../gio/gdatainputstream.c \
	../gio/gdataoutputstream.c \
	../gio/gdbusaddress.c \
	../gio/gdbusauth.c \
	../gio/gdbusauthmechanism.c \
	../gio/gdbusauthmechanismsha1.c \
	../gio/gdbusconnection.c \
	../gio/gdbusmessage.c \
	../gio/gdbusmethodinvocation.c \
	../gio/gdbusprivate.c \
	../gio/gdbusproxy.c \
	../gio/gdbusserver.c \
	../gio/gdbus-tool.c \
	../gio/gdesktopappinfo.c \
	../gio/gdrive.c \
	../gio/gdummyfile.c \
	../gio/gdummytlsbackend.c \
	../gio/gemblem.c \
	../gio/gemblemedicon.c \
	../gio/gfileattribute.c \
	../gio/gfile.c \
	../gio/gfileenumerator.c \
	../gio/gfileicon.c \
	../gio/gfileinfo.c \
	../gio/gfileinputstream.c \
	../gio/gfileiostream.c \
	../gio/gfilemonitor.c \
	../gio/gfilenamecompleter.c \
	../gio/gfileoutputstream.c \
	../gio/gfilterinputstream.c \
	../gio/gfilteroutputstream.c \
	../gio/gicon.c \
	../gio/ginetaddress.c \
	../gio/ginetaddressmask.c \
	../gio/ginetsocketaddress.c \
	../gio/ginitable.c \
	../gio/ginputstream.c \
	../gio/gioerror.c \
	../gio/giomodule.c \
	../gio/gioscheduler.c \
	../gio/giostream.c \
	../gio/glib-compile-resources.c \
	../gio/glib-compile-schemas.c \
	../gio/gloadableicon.c \
	../gio/glocaldirectorymonitor.c \
	../gio/glocalfile.c \
	../gio/glocalfileenumerator.c \
	../gio/glocalfileinfo.c \
	../gio/glocalfileinputstream.c \
	../gio/glocalfilemonitor.c \
	../gio/glocalfileoutputstream.c \
	../gio/glocalvfs.c \
	../gio/gmemoryinputstream.c \
	../gio/gmemoryoutputstream.c \
	../gio/gmount.c \
	../gio/gmountoperation.c \
	../gio/gnativevolumemonitor.c \
	../gio/gnetworkaddress.c \
	../gio/gnetworkmonitorbase.c \
	../gio/gnetworkmonitor.c \
	../gio/gnetworkmonitornetlink.c \
	../gio/gnetworkservice.c \
	../gio/goutputstream.c \
	../gio/gpermission.c \
	../gio/gpollableinputstream.c \
	../gio/gpollableoutputstream.c \
	../gio/gpollfilemonitor.c \
	../gio/gpropertyaction.c \
	../gio/gproxyaddress.c \
	../gio/gproxyaddressenumerator.c \
	../gio/gresolver.c \
	../gio/gresource.c \
	../gio/gresourcefile.c \
	../gio/gresource-tool.c \
	../gio/gseekable.c \
	../gio/gsettingsbackend.c \
	../gio/gsettings.c \
	../gio/gsettings-tool.c \
	../gio/gsimpleaction.c \
	../gio/gsimpleasyncresult.c \
	../gio/gsimpleproxyresolver.c \
	../gio/gsocketaddress.c \
	../gio/gsocket.c \
	../gio/gsocketclient.c \
	../gio/gsocketconnectable.c \
	../gio/gsocketconnection.c \
	../gio/gsocketinputstream.c \
	../gio/gsocketlistener.c \
	../gio/gsocketoutputstream.c \
	../gio/gsocks4aproxy.c \
	../gio/gsocks5proxy.c \
	../gio/gsubprocess.c \
	../gio/gtcpconnection.c \
	../gio/gtcpwrapperconnection.c \
	../gio/gtestdbus.c \
	../gio/gthemedicon.c \
	../gio/gthreadedresolver.c \
	../gio/gthreadedsocketservice.c \
	../gio/gtlscertificate.c \
	../gio/gtlsclientconnection.c \
	../gio/gtlsconnection.c \
	../gio/gtlsfiledatabase.c \
	../gio/gtlspassword.c \
	../gio/gtlsserverconnection.c \
	../gio/gunionvolumemonitor.c \
	../gio/gunixconnection.c \
	../gio/gunixcredentialsmessage.c \
	../gio/gunixinputstream.c \
	../gio/gunixmount.c \
	../gio/gunixmounts.c \
	../gio/gunixoutputstream.c \
	../gio/gunixsocketaddress.c \
	../gio/gunixvolume.c \
	../gio/gunixvolumemonitor.c \
	../gio/gvfs.c \
	../gio/gvolume.c \
	../gio/gvolumemonitor.c \
	../gio/gwin32appinfo.c \
	../gio/gwin32inputstream.c \
	../gio/gwin32outputstream.c \
	../gio/gzlibcompressor.c \
	../gio/gzlibdecompressor.c \
	../gio/tests/gdbus-daemon.c \
	../gio/win32/gwinhttpfile.c \
	../glib/gatomic.c \
	../glib/gbookmarkfile.c \
	../glib/gconvert.c \
	../glib/gdatetime.c \
	../glib/gdir.c \
	../glib/gfileutils.c \
	../glib/giochannel.c \
	../glib/giowin32.c \
	../glib/gkeyfile.c \
	../glib/glib.py \
	../glib/gmappedfile.c \
	../glib/gmarkup.c \
	../glib/goption.c \
	../glib/gregex.c \
	../glib/gshell.c \
	../glib/gspawn.c \
	../glib/gspawn-win32.c \
	../glib/gutf8.c \
	../glib/gutils.c \
	../gobject/gbinding.c

CATALOGS =  af.gmo am.gmo an.gmo ar.gmo as.gmo ast.gmo az.gmo be.gmo be@latin.gmo bg.gmo bn.gmo bn_IN.gmo bs.gmo ca.gmo ca@valencia.gmo cs.gmo cy.gmo da.gmo de.gmo dz.gmo el.gmo en_CA.gmo en_GB.gmo en@shaw.gmo eo.gmo es.gmo et.gmo eu.gmo fa.gmo fi.gmo fr.gmo ga.gmo gl.gmo gu.gmo he.gmo hi.gmo hr.gmo hu.gmo hy.gmo id.gmo is.gmo it.gmo ja.gmo ka.gmo kk.gmo kn.gmo ko.gmo ku.gmo lt.gmo lv.gmo mai.gmo mg.gmo mk.gmo ml.gmo mn.gmo mr.gmo ms.gmo nb.gmo nds.gmo ne.gmo nl.gmo nn.gmo oc.gmo or.gmo pa.gmo pl.gmo ps.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo rw.gmo si.gmo sk.gmo sl.gmo sq.gmo sr.gmo sr@latin.gmo sr@ije.gmo sv.gmo ta.gmo te.gmo tg.gmo th.gmo tl.gmo tr.gmo ug.gmo tt.gmo uk.gmo vi.gmo wa.gmo xh.gmo yi.gmo zh_CN.gmo zh_HK.gmo zh_TW.gmo
CATOBJEXT = .gmo
INSTOBJEXT = .mo

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(GETTEXT_PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(GETTEXT_PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	$(AM_V_GEN) file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) $(MSGFMT_OPTS) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-yes

all-yes: $(CATALOGS)
all-no:

$(srcdir)/$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(GETTEXT_PACKAGE) \
	  --msgid-bugs-address='http://bugzilla.gnome.org/enter_bug.cgi?product=glib&keywords=I18N+L10N&component=general' \
	  --add-comments --keyword=_ --keyword=N_ \
	  --keyword=C_:1c,2 \
	  --keyword=NC_:1c,2 \
	  --keyword=g_dcgettext:2 \
	  --keyword=g_dngettext:2,3 \
	  --keyword=g_dpgettext2:2c,3 \
	  --flag=N_:1:pass-c-format \
	  --flag=C_:2:pass-c-format \
	  --flag=NC_:2:pass-c-format \
	  --flag=g_dngettext:2:pass-c-format \
          --flag=g_strdup_printf:1:c-format \
          --flag=g_string_printf:2:c-format \
          --flag=g_string_append_printf:2:c-format \
          --flag=g_error_new:3:c-format \
          --flag=g_set_error:4:c-format \
          --flag=g_markup_printf_escaped:1:c-format \
          --flag=g_log:3:c-format \
          --flag=g_print:1:c-format \
          --flag=g_printerr:1:c-format \
          --flag=g_printf:1:c-format \
          --flag=g_fprintf:2:c-format \
          --flag=g_sprintf:2:c-format \
          --flag=g_snprintf:3:c-format \
          --flag=g_scanner_error:2:c-format \
          --flag=g_scanner_warn:2:c-format \
	  $(POTFILES) \
	&& test ! -f $(GETTEXT_PACKAGE).po \
	   || ( rm -f $(srcdir)/$(GETTEXT_PACKAGE).pot \
		&& mv $(GETTEXT_PACKAGE).po $(srcdir)/$(GETTEXT_PACKAGE).pot )

install: install-exec install-data
install-exec:
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	$(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  $(MKINSTALLDIRS) $$dir; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  rm -f $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	fi

check: all

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(GETTEXT_PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = ../$(GETTEXT_PACKAGE)-$(VERSION)/$(subdir)
dist distdir: $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(GETTEXT_PACKAGE).pot
	tmpdir=`pwd`; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.po $(GETTEXT_PACKAGE).pot -o $$tmpdir/$$lang.new.po; then \
	    if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	      rm -f $$tmpdir/$$lang.new.po; \
            else \
	      if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	        :; \
	      else \
	        echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	        rm -f $$tmpdir/$$lang.new.po; \
	        exit 1; \
	      fi; \
	    fi; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$tmpdir/$$lang.new.po; \
	  fi; \
	done

# POTFILES is created from POTFILES.in by stripping comments, empty lines
# and Intltool tags (enclosed in square brackets), and appending a full
# relative path to them
POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' 						\
		    -e "s/^\[.*\] +//" 					\
		    -e '/^[ 	]*$$/d' 				\
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in	\
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

Makefile: Makefile.in.in ../config.status POTFILES
	cd .. \
	  && $(SHELL) ./config.status $(subdir)/$@.in

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
