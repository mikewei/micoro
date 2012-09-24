if GCC
AM_CFLAGS += -Wall
endif

AM_CPPFLAGS = -I$(top_srcdir)/src

LDADD = libperf_main.la
LDADD += ../src/libmicoro.la
LDADD += -lpthread

noinst_LTLIBRARIES = libperf_main.la

libperf_main_la_SOURCES = perf_main.c
libperf_main_la_LDFLAGS = -static

noinst_PROGRAMS = \
	asm_lock_perf \
	coro_cr_perf \
	coro_sw_perf \
	lock_perf \
	mm_perf \
	testcase \
	test_coro_mt

asm_lock_perf_SOURCES = asm_lock_perf.c
coro_cr_perf_SOURCES = coro_cr_perf.c
coro_sw_perf_SOURCES = coro_sw_perf.c
lock_perf_SOURCES = lock_perf.c
mm_perf_SOURCES = mm_perf.c
testcase_SOURCES = testcase.c
test_coro_mt_SOURCES = test_coro_mt.c
