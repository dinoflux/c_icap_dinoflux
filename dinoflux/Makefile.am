
INCLUDES = -I$(top_srcdir)
CONFIGDIR = @CONFDIR@

cicapmoduledir=@MODSDIR@
cicapmodule_LTLIBRARIES=dinoflux.la

dinoflux_la_LIBADD = @MODULES_LIBADD@ -L/opt/drainware/lib
dinoflux_la_CFLAGS = -I../../include -I/opt/drainware/include/c_icap -I/opt/drainware/include -fexceptions
dinoflux_la_CPPFLAGS = -I../../include -I/opt/drainware/include/c_icap -I/opt/drainware/include -fexceptions
dinoflux_la_LDFLAGS =  -module -avoid-version -pthread 
dinoflux_la_SOURCES = dinoflux.cpp 

install-data-local:
	$(INSTALL) dinoflux.conf $(DESTDIR)$(CONFIGDIR)/dinoflux.conf.default
	if test ! -f $(DESTDIR)$(CONFIGDIR)/dinoflux.conf; then $(INSTALL) dinoflux.conf $(DESTDIR)$(CONFIGDIR)/dinoflux.conf; fi


EXTRA_DIST= dinoflux.h makefile.w32 dinoflux.def dinoflux.conf
