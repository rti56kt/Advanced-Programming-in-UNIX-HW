mov ebx, [0x600000]
lea eax, [ebx*4+ebx]
mov ebx, [0x600004]
sub ebx, 3
idiv ebx
mov [0x600008], eax
done: