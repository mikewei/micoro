TEST_BIN = testcase
PERF_BIN = lock_perf mm_perf coro_cr_perf coro_sw_perf
BIN = $(TEST_BIN) $(PERF_BIN)
CFLAGS = -Wall -MMD $(ARCH)
INCLIBS = -I.. -L.. -lcoro -lpthread

all: $(BIN)

all32: ARCH = -m32
all32: all

$(BIN): libs

$(TEST_BIN): CFLAGS += -g
$(TEST_BIN): %: %.c
	gcc $(CFLAGS) -o $@ $< $(INCLIBS)

$(PERF_BIN): CFLAGS += -g -O2
$(PERF_BIN): %: %.c perf_main.c
	gcc $(CFLAGS) -o $@ $(wildcard $^) $(INCLIBS)

-include *.d

libs:
	make -C .. libs

clean:
	rm -f $(BIN) *.o *.d

.PHONY: all clean libs
