all: hw2

CC = g++
DBGFLAGS = -g -Wall
LIBFLAGS = -shared -fPIC -ldl
LARGEFILEFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

hw2: hw2.cpp hw2lib.cpp
	$(CC) hw2.cpp $(DBGFLAGS) $(LARGEFILEFLAGS) -O -o sandbox
	$(CC) hw2lib.cpp $(DBGFLAGS) $(LIBFLAGS) -o sandbox.so

clean:
	rm -f sandbox sandbox.so
