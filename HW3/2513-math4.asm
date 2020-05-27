mov eax, -1
imul dword ptr [0x600004]
idiv dword ptr [0x600008]
mov [0x60000c], edx
mov eax, -5
imul dword ptr [0x600000]
cdq
idiv dword ptr [0x60000c]
mov dword ptr [0x60000c], eax
done: