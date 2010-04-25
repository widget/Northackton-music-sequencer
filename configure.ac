#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT(music-seq, 0.1, northackton@groups.google.com)
AM_INIT_AUTOMAKE(music-seq, 0.1)
AC_CONFIG_SRCDIR([seqcircles.cpp])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX

PKG_CHECK_MODULES(OPENCV, opencv)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcv':
AC_CHECK_LIB([cv], [Canny])
# FIXME: Replace `main' with a function in `-lhighgui':
AC_CHECK_LIB([highgui], [imshow])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
