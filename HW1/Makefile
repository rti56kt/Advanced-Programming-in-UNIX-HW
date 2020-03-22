all: hw1

CC = g++
CFLAGS = -g -Wall
SUDO = sudo
MODFLAGS = u+s

hw1: hw1.cpp
	$(CC) hw1.cpp $(CFLAGS) -O -o hw1

debug: hw1.cpp
	$(SUDO) $(CC) hw1.cpp $(CFLAGS) -O -o hw1
	$(SUDO) chmod $(MODFLAGS) hw1

clean:
	rm -f hw1

clean-debug:
	$(SUDO) rm -f hw1
