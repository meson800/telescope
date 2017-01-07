AC_INIT([telescope], [0.1], [meson800@gmail.com])
AM_PROG_AR
LT_INIT
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11
AC_CHECK_LIB(noise, libnoise_is_present, , AC_MSG_ERROR([Cannot compile telescope without libnoise. Try installing libnoise]))
AC_CHECK_HEADER([fstream], , AC_MSG_ERROR([Cannot compile telescope without header "fstream"]))
AC_CHECK_HEADER([iostream], , AC_MSG_ERROR([Cannot compile telescope without header "iostream"]))
AM_OPTIONS_WXCONFIG
reqwx=3.0.0
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
		AC_MSG_ERROR([
				wxWidgets must be installed on your system.
				 
				 		Please check that wx-config is in path, the directory
								where wxWidgets libraries are installed (returned by
										'wx-config --libs' or 'wx-config --static --libs' command)
										is in LD_LIBRARY_PATH or equivalent variable and
												wxWidgets version is $reqwx or above.
														])
		fi
		 
CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"
AC_PROG_RANLIB
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
