CFLAGS += -Wall -Wextra -Wshadow -pedantic -std=c99

all: mmfetch
mmfetch: mmfetch.o
mmfetch.o: mmfetch.c arg.h

clean:
	rm -f *.o mmfetch

