#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([micoro], [1.0], [bin@vip.qq.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/micoro.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_HOST
AS_CASE(["$host_cpu"], 
		[x86_64], [],
		[i?86], []
		[AC_MSG_ERROR([only support x86 now!])])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
LT_INIT

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_CONDITIONAL(GCC, test "$GCC" = yes)
AS_IF([test "x$GCC" = xyes], []
	  [AC_MSG_WARN([***** non-gcc is not tested, maybe fail when compiling! *****])])

AC_SUBST([AM_CFLAGS], ["$CFLAGS"])
CFLAGS=""

#AC_PROG_INSTALL
#AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([locale.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([alarm gettimeofday memset munmap setlocale])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT
