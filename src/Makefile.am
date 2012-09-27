if GCC
AM_CFLAGS += -Wall
endif

include_HEADERS = \
	micoro.h

lib_LTLIBRARIES = \
	libmicoro.la \
	libmicoro_mt.la

libmicoro_SOURCES = \
	coro_comm.h \
	coro_mm.h \
	coro_mm_malloc.c \
	coro_mm_pool.c \
	coro_sched.c \
	micoro.h \
	mt_utils.c \
	mt_utils.h

libmicoro_la_SOURCES = $(libmicoro_SOURCES)
libmicoro_la_LIBADD = libtmp.la
libmicoro_la_LDFLAGS = -static

libmicoro_mt_la_SOURCES = $(libmicoro_SOURCES)
libmicoro_mt_la_CPPFLAGS = -DMICORO_MT
libmicoro_mt_la_LIBADD = libtmp.la
libmicoro_mt_la_LDFLAGS = -static

noinst_LTLIBRARIES = libtmp.la

libtmp_la_SOURCES = \
	coro_sw_O0.c

libtmp_la_CFLAGS = $(AM_CFLAGS) -O0
libtmp_la_LDFLAGS = -static

