all: hw2

CC = g++
DBGFLAGS = -g -Wall
LIBFLAGS = -shared -fPIC -ldl

hw2: hw2.cpp hw2lib.cpp
	$(CC) hw2.cpp $(DBGFLAGS) -O -o sandbox
	$(CC) hw2lib.cpp $(DBGFLAGS) $(LIBFLAGS) -o sandbox.so

clean:
	rm -f sandbox sandbox.so
