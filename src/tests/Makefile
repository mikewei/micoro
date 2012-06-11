TEST_BIN = testcase test_coro_mt
PERF_BIN = $(patsubst %_perf.c, %_perf, $(wildcard *_perf.c))
BIN = $(TEST_BIN) $(PERF_BIN)
CORO_LIB = ../libcoro.a
CFLAGS = -Wall -MMD $(ARCH)
INCLIBS = -I.. -L.. -lcoro -lpthread
LIB_TARGET = libs

all: $(BIN)

all32: ARCH = -m32
all32: LIB_TARGET = libs32
all32: all

$(BIN): $(CORO_LIB)

$(TEST_BIN): CFLAGS += -g
$(TEST_BIN): %: %.c
	gcc $(CFLAGS) -o $@ $< $(INCLIBS)

$(PERF_BIN): CFLAGS += -g -O2
$(PERF_BIN): %: %.c perf_main.c
	gcc $(CFLAGS) -o $@ $(wildcard $^) $(INCLIBS)

-include *.d

$(CORO_LIB): libs
	make -C .. $(LIB_TARGET)

clean:
	rm -f $(BIN) *.o *.d

.PHONY: all clean libs
