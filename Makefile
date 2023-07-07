CC=gcc
DEBUG = -g
CFLAGS = -Wall -std=c99 -pedantic -c -O0 $(DEBUG)
LFLAGS = -Wall -std=c99 -pedantic -lm $(DEBUG)

all: raytrace raycast.o PPM.o v3math.o

ALL: all

raytrace: raycast.o PPM.o v3math.o
	$(CC) $(LFLAGS) raycast.o PPM.o v3math.o -o raytrace

raycast.o:
	$(CC) $(CFLAGS) $(DEBUG) raycast.c raycast.h

PPM.o:
	$(CC) $(CFLAGS) $(DEBUG) PPM.c PPM.h

v3math.o:
	$(CC) $(CFLAGS) $(DEBUG) v3math.c v3math.h

clean:
	rm -f *.o raytrace
