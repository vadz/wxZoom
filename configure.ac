dnl Process this file with autoconf to produce a configure script.
AC_REVISION($Id$)dnl

dnl ---------------------------------------------------------------------------
dnl Name:       configure.ac
dnl Purpose:    autoconf source for wxZoom makefile generation
dnl Author:     Vadim Zeitlin
dnl Created:    01.12.00
dnl Copyright:  (c) 2000 Vadim Zeitlin <vadim@wxwindows.org>
dnl Licence:    wxWindows licence
dnl ---------------------------------------------------------------------------

dnl generic init
AC_INIT([wxZoom], [0.03])
AC_CONFIG_SRCDIR([src/wxZoom.cpp])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])

dnl check for programs we use
AC_PROG_CXX

dnl check for wx-config
AM_OPTIONS_WXCONFIG
AM_PATH_WXCONFIG(2.1.1, WXCONFIG=1)
if test "$WXCONFIG" != 1; then
    AC_MSG_ERROR([
            Please check that wx-config is in path, the directory
            where wxWindows libraries are installed (returned by
            'wx-config --libs' command) is in LD_LIBRARY_PATH or
            equivalent variable and wxWindows is version 2.2.1 or above.
    ])
fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

dnl define outputs
AM_CONFIG_HEADER(config.h:config.h.in)
AC_OUTPUT([
            Makefile
            src/Makefile
          ])
