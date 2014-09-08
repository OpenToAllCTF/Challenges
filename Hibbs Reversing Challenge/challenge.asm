.586
.model flat, stdcall
option casemap:none
assume fs:nothing

.data
output_buffer byte 32 dup (?)

.code
Challenge proc
    push ebp
    mov ebp, esp

    mov eax, esp
    push eax
    pop edx
    cmp eax, edx
    jmp __skipJunkCode01

    jmp __skipJunkCode01 + 03h
    jmp __skipJunkCode01 + 02h
    jmp __skipJunkCode01 + 01h
    jmp __skipJunkCode01 + 02h
    jmp __skipJunkCode01 + 03h
    jmp __skipJunkCode01 + 04h
    jmp __skipJunkCode01 + 05h

    mov eax, offset __skipJunkCode01
    push eax

    xor dword ptr ds:[eax], 00h
    inc eax

    xor dword ptr ds:[eax], 00h
    inc eax

    xor dword ptr ds:[eax], 00h
    inc eax

    xor dword ptr ds:[eax], 00h
    add eax, 10h

    xor dword ptr ds:[eax], 00h
    add eax, 10h

    mov eax, offset __skipJunkCode01

    pop eax
    call eax

    xor eax, eax
    pop ebp
    retn

__skipJunkCode01:
    ; the following is the flag: 0p3nT04LLcTf!\0
    mov edx, offset output_buffer - 01h
    mov ecx, 0DEADC0DEh

    ; 0  -> eax = 71h
    mov eax, 71h
    int 03h
    mov byte ptr ds:[edx], al

    ; p  -> eax = 13h
    mov eax, 13h
    int 03h
    mov byte ptr ds:[edx], al

    ; 3  -> eax = 73h
    mov eax, 73h
    int 03h
    mov byte ptr ds:[edx], al

    ; n  -> eax = 62h
    mov eax, 62h
    int 03h
    mov byte ptr ds:[edx], al

    ; T  -> eax = 55h
    mov eax, 55h
    int 03h
    mov byte ptr ds:[edx], al

    ; 0  -> eax = 61h
    mov eax, 61h
    int 03h
    mov byte ptr ds:[edx], al

    ; 4  -> eax = 57h
    mov eax, 57h
    int 03h
    mov byte ptr ds:[edx], al

    ; L  -> eax = 49h
    mov eax, 49h
    int 03h
    mov byte ptr ds:[edx], al

    ; L  -> eax = 49h
    mov eax, 49h
    int 03h
    mov byte ptr ds:[edx], al

    ; c  -> eax = 11h
    mov eax, 11h
    int 03h
    mov byte ptr ds:[edx], al

    ; T  -> eax = 12h
    mov eax, 12h
    int 03h
    mov byte ptr ds:[edx], al

    ; f  -> eax = 24h
    mov eax, 24h
    int 03h
    mov byte ptr ds:[edx], al

    ; !  -> eax = 14h
    mov eax, 14h
    int 03h
    mov byte ptr ds:[edx], al

    ; \0 -> eax = 27h
    mov eax, 27h
    int 03h
    mov byte ptr ds:[edx], al

    xor eax, eax
    mov byte ptr ds:[edx], al

    xor esp, esp
    xor ebp, ebp
    jmp ebp
Challenge endp

end
