CC = gcc
CFLAGS = -g -pedantic -Wall -Wextra

port_scanner: main.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


run: port_scanner
	./port_scanner 89.190.46.160 1 3000

.PHONY: all run clean zip
