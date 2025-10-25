# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
LDFLAGS = -lrt

all: producer consumer

producer: producer.c shared.h
	$(CC) $(CFLAGS) producer.c -o producer $(LDFLAGS)

consumer: consumer.c shared.h
	$(CC) $(CFLAGS) consumer.c -o consumer $(LDFLAGS)

clean:
	rm -f producer consumer
