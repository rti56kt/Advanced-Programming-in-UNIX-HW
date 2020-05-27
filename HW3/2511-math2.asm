mov eax, -1
imul dword ptr [0x600000]
imul dword ptr [0x600004]
add eax, [0x600008]
done: