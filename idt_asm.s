extern interrupt_handler
global interrupt_handler_32
global interrupts_load_idt

interrupt_handler_32:
    push    dword 32                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler

common_interrupt_handler:               ; the common parts of the generic interrupt handler
    ; save the registers
    push    eax
    push    ebx
    push    ecx
    push    edx
    ;push    esp
    push    ebp
    push    esi
    push    edi
    ; call the C function
    call    interrupt_handler
    ; restore the registers
    pop     edi
    pop     esi
    pop     ebp
    ;pop     esp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    ; restore the esp
    add     esp, 8
    ; return to the code that got interrupted
    iret

interrupts_load_idt:
    mov     eax, [esp+4]    ; load the address of the IDT into register eax
    lidt    [eax]             ; load the IDT
    ret                     ; return to the calling function
    