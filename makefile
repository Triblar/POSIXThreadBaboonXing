CC = gcc
CFLAGS = -std=c11 -ggdb
OUT = as7.o
SRC = as7.c

all:	pThreads

pThreads:
	$(CC) -pthread -o $(OUT) $(SRC) $(CFLAGS)
	chmod 775 $(OUT)

clean:
	rm -f $(OUT)
