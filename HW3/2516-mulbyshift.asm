mov eax, [0x600000]
shl eax, 1
mov ebx, eax
shl eax, 2
add ebx, eax
shl eax, 1
add ebx, eax
mov [0x600004], ebx
done: