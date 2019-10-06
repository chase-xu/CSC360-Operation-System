.phony all:
all: A1

sample: A1.c
	gcc A1.c -ltermcap -o A1

.PHONY clean:
clean:
	-rm -rf *.o *.exe
