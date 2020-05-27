sub [0x600008], ebx
mov eax, -1
imul dword ptr [0x600004]
imul dword ptr [0x600000]
cdq
idiv dword ptr [0x600008]
mov dword ptr [0x600008], eax
done: