CC=gcc
CFLAGS=-Wall -std=c99 -Iinclude
LDFLAGS=-Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC=main.c
OUT=game

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)

