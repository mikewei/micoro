LIB = libcoro.a
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -g -Wall -MMD -O2 $(ARCH)

all: libs tests

all32: libs32 tests32

libs: $(LIB)

libs32: ARCH = -m32
libs32: $(LIB)

tests:
	make -C tests all

tests32:
	make -C tests all32

$(LIB): $(OBJ)
	ar r $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $<

coro_sw_O0.o: coro_sw_O0.c
	gcc -Wall -O0 $(ARCH) -c $<

-include *.d

clean:
	rm -f $(LIB) $(BIN) *.o *.d
	make -C tests clean

.PHONY: all clean libs libs32 tests tests32
