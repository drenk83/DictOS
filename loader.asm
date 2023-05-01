[BITS 16]
[ORG 0x7c00]

start:
    mov ax, cs ; Сохранение адреса сегмента кода в ax
    mov ds, ax ; Сохранение этого адреса как начало сегмента данных
    mov ss, ax ; И сегмента стека
    mov sp, start

    call user_input
    call the_end

user_input:
    ; saves video buffer addr in %edi
    mov edi, 0xb8000

    mov edx, selected_letters
    mov ecx, alphabet

    call clean_screen

    mov esi, user_instruction
    call video_puts

    mov esi, edx
    call video_puts

    mov ah, 0x00
    int 0x16

    cmp al, 0x0D
    jz work_with_memory

    mov ebx, 0
    mov edx, 26  ; size alphabet
check_in_alphabet:
    cmp al, [ecx + ebx]
    jz change

    inc ebx
    cmp ebx, edx
    jnz check_in_alphabet
    jmp user_input
change:
    mov edx, selected_letters
    cmp al, [edx + ebx]
    jnz set_letter
    mov al, 0x5F
set_letter:
    mov [edx + ebx], al
    jmp user_input

work_with_memory:
    call clean_screen
    mov edx, 0x9000
    mov ecx, 0x0
    mov bx, selected_letters
memory_loop:
    mov ax, [bx]
    mov [edx], ax
    add bx, 2
    add edx, 2
    add ecx, 2

    cmp ecx, 0x1a
    jnz memory_loop
    ret

input:
    mov ah,0
    int 0x16
    cmp al, 0x0D
    jne input
    mov ax, 0x03
    int 0x10

clean_screen:
    mov ah,0
    mov al, 2
    int 0x10
    ret

the_end:
    mov ax,0x1000
    mov es,ax 
    mov bx,0x00 
    mov ah,0x02     
    mov dl,1        
    mov dh,0x00     
    mov cl,0x01     
    mov ch,0x00     
    mov al, 18     
    int 0x13

    cli

    lgdt [gdt_info]

    in al, 0x92
    or al, 2
    out 0x92, al

    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x8:protected_mode

gdt:
; Нулевой дескриптор
    db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ; Сегмент кода: base=0, size=4Gb, P=1, DPL=0, S=1(user),
    ; Type=1(code), Access=00A, G=1, B=32bit
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x9A, 0xCF, 0x00
    ; Сегмент данных: base=0, size=4Gb, P=1, DPL=0, S=1(user),
    ; Type=0(data), Access=0W0, G=1, B=32bit
    db 0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xCF, 0x00
    gdt_info: ; Данные о таблице GDT (размер, положение в памяти)
dw gdt_info - gdt
    ; Размер таблицы (2 байта)
    dw gdt, 0
    ; 32-битный физический адрес таблицы.

video_puts:
    mov al, [esi]
    test al, al
    jz video_end_puts

    mov ah, 0x0e
    int 0x10

    add esi, 1
    jmp video_puts

video_end_puts:
    ret

alphabet:
    db "abcdefghijklmnopqrstuvwxyz", 0

selected_letters:
    db "__________________________", 0

user_instruction:
    db "Type letters from which the desired words begin: ", 0

[BITS 32]
protected_mode:
    mov ax, 0x10 
    mov es, ax
    mov ds, ax
    mov ss, ax

    call 0x10000

    inf_loop:
        jmp inf_loop ; Бесконечный цикл
        times (512 - ($ - start) - 2) db 0 
        db 0x55, 0xAA