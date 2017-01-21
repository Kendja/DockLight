PACKAGE=docklight
VERSION=1.0-11


AC_INIT([docklight], [1.0], [bug-automake@gnu.org])
# this flags are for automake
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
# find & probe c compiler
AC_PROG_CC
# find & probe g++ compiler
AC_PROG_CXX
# make config headers
AC_CONFIG_HEADERS([config.h])
# configure create makefiles
AC_CONFIG_FILES([
 Makefile
 data/Makefile
 po/Makefile.in	
 src/Makefile
])

dnl ======================================================
IT_PROG_INTLTOOL([0.35.0])  

GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])

PROGRAMNAME_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PROGRAMNAME_LOCALEDIR)
dnl ======================================================

AC_CONFIG_MACRO_DIR([m4])



#AC_SUBST_FILE([ouptput_bin])
          ouptput_bin=$srcdir/bin/docklight

#AC_DEFINE_UNQUOTED([DATADIR], ["$datadir"], [/usr/lib/DockLight])


# Checks for programs.
AC_PROG_CXX

dnl ==========================================================================
echo "
	docklight Version       $VERSION
	prefix:                 ${prefix}
	source code location:	${srcdir}
	compiler:		${CC}
	top_srcdir  		${top_srcdir}
	bin dir			$(bindir)
"


AC_OUTPUT
