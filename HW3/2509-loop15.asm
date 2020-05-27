mov cx, 15
mov r8, 0

L:
mov al, byte ptr [0x600000+r8]
or al, 0b100000
mov byte ptr [0x600010+r8], al
inc r8
loop L
done: