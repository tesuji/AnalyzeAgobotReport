__inline bool IsSICELoaded() {
 _asm {
     mov ah, 0x43
     int 0x68
     cmp ax, 0x0F386 // Will be set by all system debuggers.
     jz out_

     xor ax, ax
     mov es, ax
     mov bx, word ptr es:[0x68*4]
     mov es, word ptr es:[0x68*4+2]
     mov eax, 0x0F43FC80
     cmp eax, dword ptr es:[ebx]
     jnz out_
     jmp normal_
normal_:
     xor eax, eax
     leave
     ret
out_:
     mov eax, 0x1
     leave
     ret
 }
    return false;
}

__inline bool IsODBGLoaded() {
    char *caption="DAEMON";
    _asm {
        push 0x00
        push caption

        mov eax, fs:[30h]               // pointer to PEB
        movzx eax, byte ptr[eax+0x2]
        or al,al
        jz normal_
        jmp out_
normal_:
        xor eax, eax
        leave
        ret
out_:
        mov eax, 0x1
        leave
        ret
    }
}

__inline BOOL IsSoftIceNTLoaded() {
    HANDLE hFile=CreateFile( "\\\\.\\NTICE",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile!=INVALID_HANDLE_VALUE) { CloseHandle(hFile); return TRUE; }
    return FALSE; }

__inline bool IsBPX(void *address) {
    _asm {
        mov esi, address    // load function address
        mov al, [esi]       // load the opcode
        cmp al, 0xCC        // check if the opcode is CCh
        je BPXed            // yes, there is a breakpoint

        // jump to return TRUE
        xor eax, eax        // FALSE,
        jmp NOBPX           // no breakpoint
BPXed:
        mov eax, 1          // breakpoint found
NOBPX:
    }
}
