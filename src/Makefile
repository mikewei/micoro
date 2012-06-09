LIB = libcoro.a
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
CFLAGS = -g -Wall -MMD -O2 $(ARCH)

all: libs tests

all32: ARCH = -m32
all32: all

libs: $(LIB)

tests:
	make -C tests $(MAKECMDGOALS)

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

.PHONY: all clean libs tests
