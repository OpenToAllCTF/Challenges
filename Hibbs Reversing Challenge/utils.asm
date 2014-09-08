.586
.model flat, stdcall

option casemap:none
assume fs:nothing

.code
CustomIsDebuggerPresent proc
    push ebp
    mov ebp, esp

    mov eax, dword ptr fs:[30h]
    movzx eax, byte ptr ds:[eax + 02h]

    pop ebp
    retn
CustomIsDebuggerPresent endp

end
