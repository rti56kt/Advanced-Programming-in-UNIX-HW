from pwn import *
import binascii as b

context.arch = "x86_64"

r = remote("aup.zoolab.org", 2530)

for j in range(10):
    z = r.recvuntil("Your answer: ").decode("utf-8")
    print(z)
    z = z.split()

    instructions = disasm(b.a2b_hex(z[-3])).split("\n")
    for i, ins in enumerate(instructions):
        instructions[i] = " ".join(ins[32:].split())

    result = "\n".join(instructions) + "\n"
    result = b.b2a_hex(result.encode("utf-8"))
    print(result.decode("utf-8"))
    r.sendline(result.decode("utf-8"))
r.interactive()
