CC = gcc
CFLAGS = -Wall -Wextra -std=c11

banker: banker.c
	$(CC) $(CFLAGS) banker.c -o banker

clean:
	rm -f banker

.PHONY: clean