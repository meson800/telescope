INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_TELESCOPE telescope)

FIND_PATH(
    TELESCOPE_INCLUDE_DIRS
    NAMES telescope/api.h
    HINTS $ENV{TELESCOPE_DIR}/include
        ${PC_TELESCOPE_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    TELESCOPE_LIBRARIES
    NAMES gnuradio-telescope
    HINTS $ENV{TELESCOPE_DIR}/lib
        ${PC_TELESCOPE_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TELESCOPE DEFAULT_MSG TELESCOPE_LIBRARIES TELESCOPE_INCLUDE_DIRS)
MARK_AS_ADVANCED(TELESCOPE_LIBRARIES TELESCOPE_INCLUDE_DIRS)

