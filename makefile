INCLUDES=subpng.h
BIN_SRC=pngtest.c png2apng.c
BINARIES=$(BIN_SRC:.c=)
OBJ_SRC=subpng.c
OBJECTS=$(OBJ_SRC:.c=.o)

CC=gcc
C_FLAGS=-Wall -I.
LD_FLAGS=$(OBJECTS) -lz

ALL: $(BINARIES) $(OBJECTS)

%.o: %.c $(INCLUDES)
	$(CC) -c $(C_FLAGS) $< -o $@

$(BINARIES): $(INCLUDES) $(OBJECTS)
	gcc $(C_FLAGS) -o $@ $(LD_FLAGS) $@.c

clean:
	echo "$(BIN_SRC)"
	rm -rf $(BINARIES) $(OBJECTS) core

test:
	./pngtest test.png

test2:
	./pngtest test.apng

.PHONY: clean test test2
