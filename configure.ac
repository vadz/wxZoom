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
AM_INIT_AUTOMAKE

dnl check for programs we use
AC_PROG_CXX

dnl check for wx-config
WX_PATH_WXCONFIG(2.1.1, WXCONFIG=1)
if test "$WXCONFIG" != 1; then
    AC_MSG_ERROR([
            Please check that wx-config is in path, the directory
            where wxWindows libraries are installed (returned by
            'wx-config --libs' command) is in LD_LIBRARY_PATH or
            equivalent variable and wxWindows is version 2.2.1 or above.
    ])
fi

CXXFLAGS="${CXXFLAGS} ${WX_CFLAGS}"
LDFLAGS="${LDFLAGS} ${WX_LIBS}"

dnl define outputs
AM_CONFIG_HEADER(config.h:config.h.in)
AC_OUTPUT([
            Makefile
            src/Makefile
          ])
