
#---------------------------------------------------------
#
PACKAGE=docklight
#
#---------------------------------------------------------
VERSION=1.0.13
INSTALLDIR=/usr/local/docklight
LOCALEDIR=/usr/lib/docklight/share/locale

dnl ======================================================
AC_INIT([docklight], [1.0.13], [bug-automake@gnu.org])
# this flags are for automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# find & probe c compiler
AC_PROG_CC
# find & probe g++ compiler
AC_PROG_CXX
# make config headers
AC_CONFIG_HEADERS([config.h])


dnl ======================================================
#AC_PREFIX_DEFAULT($INSTALLDIR)
#AC_PREFIX_PROGRAM ($INSTALLDIR)
#AC_SUBST(INSTALLDIR)
dnl ======================================================
IT_PROG_INTLTOOL([0.35.0])  

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])

# This PROGRAMNAME_LOCALEDIR variable will be used later in the Makefile.am file,
# to define a macro that will be used when you initialize gettext in your source code.
#PROGRAMNAME_LOCALEDIR=[${datadir}/locale]
#PROGRAMNAME_LOCALEDIR=[${prefix}/locale]

PROGRAMNAME_LOCALEDIR=$LOCALEDIR

#@AC_SUBST(LOCALEDIR)
AC_SUBST(PROGRAMNAME_LOCALEDIR)

dnl ======================================================
AC_CONFIG_MACRO_DIR([m4])
dnl ======================================================

# This checks for the presence of gtkmm-3.0 and defines
# GTKMM_LIBS and GTKMM_CFLAGS for use in Makefile.am files. 
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.8.0])

# This checks for the presence of libwnck-3.0  and defines
# WNCK_LIBS and WNCK_CFLAGS for use in Makefile.am files. 
PKG_CHECK_MODULES([WNCK], [libwnck-3.0 ] )

#MM_PKG_CONFIG_SUBST([GTHREAD_LIBS], [--libs libwnck-3.0])
AC_PATH_PROG(PKG_CONFIG, pkg-config)

# check X11 this add -lX11 and substiture the X11_CFLAGS and X11_LIBS variables.
AC_CHECK_LIB([X11], [XOpenDisplay])

# check wnck-3 this add -lwnck-3 and substiture the wnck-3_CFLAGS and wnck-3_LIBS variables.
AC_CHECK_LIB([wnck-3],[wnck_screen_get_default])

dnl ======================================================
#AC_SUBST_FILE([ouptput_bin])
#  ouptput_bin=$srcdir/bin/docklight
#AC_DEFINE_UNQUOTED([DATADIR], ["$datadir"], [/usr/local/DockLight])

dnl ======================================================

# configure create makefiles
AC_CONFIG_FILES([
 Makefile
 data/Makefile
 data/icons/Makefile
 po/Makefile.in	
 src/Makefile
])


#AC_SUBST_FILE([ouptput_bin])
#ouptput_bin=$srcdir/bin/docklight
#AC_DEFINE_UNQUOTED([DATADIR], ["$datadir"], [/usr/local/DockLight])
dnl ==========================================================================
echo "
	docklight Version       $VERSION
	prefix:                 ${prefix}
	source code location:	${srcdir}
	compiler:		${CC}
	top_srcdir  		${top_srcdir}
	bin dir			$(bindir)
        locale   		${PROGRAMNAME_LOCALEDIR}
"

AC_OUTPUT


