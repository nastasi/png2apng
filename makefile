#
#    Copyright (C) 2016 Matteo Nastasi <nastasi@alternativeoutput.it>
#
#    This file is part of png2apng.
#
#    png2apng is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Foobar is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
#
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

$(BINARIES): $(INCLUDES) $(OBJECTS) $(BIN_SRC)
	gcc $(C_FLAGS) -o $@ $(LD_FLAGS) $@.c

clean:
	echo "$(BIN_SRC)"
	rm -rf $(BINARIES) $(OBJECTS) core out/myout.apng

test:
	./pngtest test.png

test2:
	./pngtest test.apng

test3:
	rm -f out/myout.apng ; ./png2apng out/myout.apng img/*48px*

.PHONY: clean test test2 test3
