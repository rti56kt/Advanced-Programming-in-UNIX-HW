# Homework #3

## ptrace

In this homework, there are 5 ptrace problems on [online course practice website](https://aup.zoolab.org/challenges) (the link **may be unavailable** after the semester is end), you need to use the knowledge of ptrace to find the FLAGS, and submit to the online practice website.

### Problems

* traceme
  * Trace me, please.
  * **(Hint)** We have released a binary with debug symbols (traceme2) and uncovered parts of the source code (traceme2.c). It would reduce the difficulty of this problem. Please have a look!
* countme
  * Count how many instructions are executed.
  * The flag is `ASM{#-of-instructions}`
* capstone
  * Have fun with capstone!
  * `nc aup.zoolab.org 2530`
* syscall
  * Count how many syscalls are invoked.
  * The flag is `ASM{#-of-syscalls}`
* no more traps
  * Replace encountered 0xcc traps with the opcodes in **no_more_traps.txt**

### Problem Files

* traceme
  * 1-traceme
  * 1-traceme2
  * 1-traceme2.c
* countme
  * 2-countme
* capstone
  * `None` *(Please connect to aup.zoolab.org:2530 to see the problem)*
* syscall
  * 4-syscall
* no more traps
  * 5-no_more_traps
  * 5-no_more_traps.txt

### Solution Files

* traceme
  * 1-traceme.txt
* countme
  * 2-counter.c
* capstone
  * 3-capstone.py
* syscall
  * 4-cntsyscall.c
* no more traps
  * 5-no_cc.c
