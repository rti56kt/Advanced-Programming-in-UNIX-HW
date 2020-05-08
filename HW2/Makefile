all: hw2

CC = g++
DBGFLAGS = -g -Wall
LIBFLAGS = -shared -fPIC -ldl

hw2: hw2.cpp
	$(CC) hw2.cpp $(DBGFLAGS) -O -o sandbox
	$(CC) inject-lib.cpp $(DBGFLAGS) $(LIBFLAGS) -o sandbox.so

clean:
	rm -f sandbox sandbox.so
