mov rcx, 19
mov r8, 2
mov r9, 3
call R
jmp Exit

R:
cmp rcx, 1
jg Recur
jl Case1
mov rax, 1
jmp ExitFunc

Case1:
mov rax, 0
jmp ExitFunc

Recur:
dec rcx
push rcx
call R
mul r8
pop rcx
push rax
dec rcx
call R
mul r9
pop rdx
add rax, rdx

ExitFunc:
ret
Exit:
done: