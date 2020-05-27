mov dx, 9

OuterLoop:
mov cx, 9
lea esi, [0x600000]

InnerLoop:
mov eax, [esi]
cmp eax, [esi+4]
jl NoNeedChg
xchg eax, [esi+4]
mov [esi], eax

NoNeedChg:
add esi, 4
loop InnerLoop

dec dx
jnz OuterLoop
done: