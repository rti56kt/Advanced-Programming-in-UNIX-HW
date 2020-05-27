mov cx, 16
mov r8, 15
mov r9, 2

L:
div r9
cmp dx, 0
je L1
mov byte ptr [0x600000+r8], '1'
jmp Common

L1:
mov byte ptr [0x600000+r8], '0'

Common:
dec r8
loop L
done: