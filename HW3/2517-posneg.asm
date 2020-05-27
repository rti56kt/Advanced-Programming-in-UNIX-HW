cmp eax, 0
jge L1
jmp L2

L1:
mov dword ptr [0x600000], 1
jmp L3

L2:
mov dword ptr [0x600000], -1

L3:
cmp ebx, 0
jge L4
jmp L5

L4:
mov dword ptr [0x600004], 1
jmp L6

L5:
mov dword ptr [0x600004], -1

L6:
cmp ecx, 0
jge L7
jmp L8

L7:
mov dword ptr [0x600008], 1
jmp L9

L8:
mov dword ptr [0x600008], -1

L9:
cmp edx, 0
jge L10
jmp L11

L10:
mov dword ptr [0x60000c], 1
jmp L12

L11:
mov dword ptr [0x60000c], -1

L12:
done: