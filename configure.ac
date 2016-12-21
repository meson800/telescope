AC_INIT([telescope], [0.1], [meson800@gmail.com])
AM_PROG_AR
LT_INIT
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11
AC_CHECK_LIB(noise, libnoise_is_present, , AC_MSG_ERROR([Cannot compile telescope without libnoise. Try installing libnoise]))
AC_CHECK_LIB(ssl, SSL_library_init, , AC_MSG_ERROR([Cannot compile telescope without libssl. Try installing the libssl and libssl-dev packages]))
AC_CHECK_LIB(crypto, OPENSSL_config, , AC_MSG_ERROR([Cannot compile telescope without libcrypto. Try installing the libssl and libssl-dev packages]))
AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR([Cannot compile telescope without libpthread]))
AC_CHECK_HEADER([fstream], , AC_MSG_ERROR([Cannot compile telescope without header "fstream"]))
AC_CHECK_HEADER([iostream], , AC_MSG_ERROR([Cannot compile telescope without header "iostream"]))
AC_PROG_RANLIB
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
