BIN=png2apng

ALL: $(BIN)



png2apng: png2apng.c
	gcc -ggdb -Wall -o png2apng png2apng.c -lz

clean:
	rm -rf $(BIN)

test:
	./png2apng test.png

test2:
	./png2apng test.apng

.PHONY: clean test test2
