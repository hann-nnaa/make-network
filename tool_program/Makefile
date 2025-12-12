CC = gcc
CFLAGS = -Wall -O2

OBJS = utils.o

all: ping traceroute

ping: ping.c $(OBJS)
	$(CC) $(CFLAGS) -o ping ping.c $(OBJS)

traceroute: traceroute.c $(OBJS)
	$(CC) $(CFLAGS) -o traceroute traceroute.c $(OBJS)

utils.o: utils.c common.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o ping traceroute
