AC_DEFUN([AX_GOOD_MPROTECT], [
	AC_MSG_CHECKING([for mprotect function])
	AC_CACHE_VAL([ac_cv_good_mprotect], [
		AC_RUN_IFELSE(
			[AC_LANG_PROGRAM([
				#include <sys/mman.h>
			], [
				#ifndef MAP_ANONYMOUS
				#   define MAP_ANONYMOUS MAP_ANON
				#endif
				void *ptr;
				ptr = mmap((void*)0, 8192, PROT_READ|PROT_WRITE, 
					MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
				if (ptr == MAP_FAILED)
					return 1;
				if (mprotect(ptr, 4096, PROT_NONE) < 0)
					return 2;
				(*(char *)(ptr + 4096)) = 0;
			])],
			ac_cv_good_mprotect=yes,
			ac_cv_good_mprotect=no
		)])
	AS_IF([test "x$ac_cv_good_mprotect" = xyes],
		  [AC_DEFINE(HAVE_GOOD_MPROTECT, 1, [define if have good mprotect])])
	AC_MSG_RESULT($ac_cv_good_mprotect)
])
