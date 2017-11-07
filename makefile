1b.out: 1b.c
	gcc 1b.c -o 1b.out
all: 1b.out
clean:
	$(RM) 1b.out
