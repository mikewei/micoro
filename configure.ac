#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([micoro], [1.0], [bin@vip.qq.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/micoro.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
LT_INIT

AC_ARG_ENABLE([x86-optimize],
			  [AS_HELP_STRING([--disable-x86-optimize],
							  [do not optimize for x86, more portable but lower performace])],
			  [],
			  [enable_x86_optimize=yes])
AC_ARG_ENABLE([force-x86],
			  [AS_HELP_STRING([--enable-force-x86],
							  [force to consider cpu as x86])],
			  [],
			  [enable_force_x86=no])
AC_ARG_ENABLE([force-gcc],
			  [AS_HELP_STRING([--enable-force-gcc],
							  [force to consider cc as gcc])],
			  [],
			  [enable_force_gcc=no])

# Check for cpu.
AC_CANONICAL_HOST
AS_CASE(["$host_cpu"], 
		[x86_64], [arch_type=x86],
		[i?86], [arch_type=x86],
		[i?86-*], [arch_type=x86],
		[arch_type=other]])
AS_IF([test "x$enable_force_x86" = xyes], [arch_type=x86])
AS_IF([test "x$arch_type" != xx86], [enable_x86_optimize=no])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AS_IF([test "x$GCC" = xyes -o "x$enable_force_gcc" = xyes],
	  [use_gcc=yes],[use_gcc=no])
AM_CONDITIONAL(GCC, test "x$use_gcc" = xyes)
AC_SUBST([AM_CFLAGS], ["$CFLAGS"])
CFLAGS=""

AS_IF([test "x$use_gcc" != xyes], [enable_x86_optimize=no])
AM_CONDITIONAL(MICORO_X86_OPTIMIZE, test "x$enable_x86_optimize" = xyes)
AS_IF([test "x$enable_x86_optimize" = xyes],
	  [AC_DEFINE([MICORO_X86_OPTIMIZE], [1], [use optimized x86 implementation])])

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

AH_TOP([
#ifndef __MICORO_CONFIG_H__
#define __MICORO_CONFIG_H__
])
AH_BOTTOM([
#endif /* __MICORO_CONFIG_H__ */
])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_OUTPUT

AS_IF([test "x$enable_x86_optimize" = xyes],
	  [impl_mode=optimized-x86-asm], [impl_mode=ucontext])
AC_MSG_NOTICE([use $impl_mode implementation])

