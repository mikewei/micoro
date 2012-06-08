BIN = testcase
LIB = libcoro.a
OBJ = coro_sched.o coro_sw_O0.o coro_mm_malloc.o coro_mm_pool.o
ARCH = #-m32
CFLAGS = -g -Wall -O0 $(ARCH)

all: $(LIB) $(BIN)

$(LIB): $(OBJ)
	ar r $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $<

coro_sw_O0.o: coro_sw_O0.c
	gcc -Wall -O0 $(ARCH) -c $<

testcase: testcase.o $(LIB)
	gcc $(CFLAGS) -o $@ $< -L. -lcoro

clean:
	rm -f $(LIB) $(BIN) *.o 

.PHONY: all clean
