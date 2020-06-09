GCC = gcc
DBGFLAGS = -g -Wall
PROGS = 2-counter 4-cntsyscall 5-no_cc

all: $(PROGS)

2-counter: 2-counter.c
	$(GCC) -o $@ $(DBGFLAGS) $^

4-cntsyscall: 4-cntsyscall.c
	$(GCC) -o $@ $(DBGFLAGS) $^

5-no_cc: 5-no_cc.c
	$(GCC) -o $@ $(DBGFLAGS) $^

clean:
	rm -rf $(PROGS)