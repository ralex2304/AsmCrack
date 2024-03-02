.model tiny
.code
.286
org 100h

MaxPasswdLen	equ 128

Start:		jmp main

InputPswdHash	dw 0
CorrectPswdHash dw 0adffh ; Pltrshk

;--------------------------------------
; Recieves password from buffer, and calculates it's hash
;
; Entry: di - PasswdInput addr
;
; Destr: 	ax, bx, dx
;--------------------------------------
CalcPasswdHash	proc

@@CPHwhileClause:
		cmp byte ptr [di], 0dh ; \n
		je @@CPHendWhile

		; hash calc: crc16

		; data ^= crc&255;
		; data ^= data << 4
		mov al, [di]
		xor al, byte ptr [offset InputPswdHash + 1]
		mov ah, al
		shl ah, 4
		xor al, ah

		; t = (((unsigned short)data << 8) | ((crc>>8)&255));
		xor bx, bx
		xor dx, dx
		mov bl, al
		shl bx, 8
		mov dl, byte ptr [offset InputPswdHash]
		or bx, dx

		; t ^= (unsigned char)(data >> 4);
		xor dx, dx
		mov dl, al
		shr dl, 4
		xor bx, dx

		; t ^= ((unsigned short)data << 3);
		xor ah, ah
		shl ax, 3
		xor bx, ax

		mov word ptr [offset InputPswdHash], bx

		inc di
		jmp @@CPHwhileClause
@@CPHendWhile:
		cmp InputPswdHash, 0
		je Congrats ; if passwd is empty, congrats

		ret
		endp
;--------------------------------------

;--------------------------------------
; Recieves password from user and checks it
;
; Entry:
;
; Destr: 	ax, bx, dx
; Return:	cl = 0/1 - wrong/right
;--------------------------------------
RecievePswd	proc

		mov cl, 1

		mov di, offset PasswdInput
		mov bx, 0
		mov dx, 0

@@WhileInputBegin:
		mov ah, 08h
		int 21h ; keybd input to al

		cmp dx, MaxPasswdLen - 1
		ja @@TooManySymbols

		mov [di + bx], al
		inc bx
		inc dx

		; length reset with symbol 8dh
		mov ah, al
		and ah, 01111111b
		cmp ah, 0dh ; \n
		jne @@WIBnoReset

		mov dx, 0
@@WIBnoReset:
		cmp al, 0dh ; \n
		jne @@WhileInputBegin

		call CalcPasswdHash

		mov ax, CorrectPswdHash
		cmp InputPswdHash, ax
		je @@RPcorrectPswd

		mov cl, 0
@@RPcorrectPswd:

		; correct ret addr to skip PasswdInput data
		pop ax
		add ax, MaxPasswdLen + 1
		push ax

		ret

@@TooManySymbols:
		mov ah, 09h
		mov dx, offset TooLongStr
		int 21h			; puts(TooLongStr)

		mov ax, 4c01h
		int 21h			; exit(al = 1)
		endp
;--------------------------------------

Congrats:	jmp @@CorrectPasswd

main:
		mov ah, 09h
		mov dx, offset GreetStr
		int 21h			; puts(GreetStr)

		call RecievePswd ; rets to command after input/fake code field

PasswdInput: 	; fake code (data field) ==========================================================
		cmp cl, 1
		jne @@WrongPasswdFake

@@CorrectPasswdFake:
		mov dx, offset AccessGrantedStr
		jmp @@endifFake
@@WrongPasswdFake:
		mov dx, offset AccessDeniedStr
@@endifFake:
		mov ah, 09h
		int 21h

		xor bx, bx
		add bx, 153d
		shr bx, 2

		call CalcPasswdHash

		mov ax, 4c00h
		int 21h			; exit(al = 0)

		db "kto prochital, tot loh"

PasswdInputFakeCodeEnd:
		db (MaxPasswdLen - (PasswdInputFakeCodeEnd - PasswdInput) + 1) dup(0deh)
		; input/fake code field end =======================================================

		cmp cl, 1
		jne @@WrongPasswd
@@CorrectPasswd:
		mov dx, offset AccessGrantedStr
		jmp @@endif
@@WrongPasswd:
		mov dx, offset AccessDeniedStr
@@endif:
		mov ah, 09h
		int 21h

		mov ax, 4c00h
		int 21h			; exit(al = 0)


GreetStr	 db "Enter password:",  0dh, 0ah, "$"
AccessGrantedStr db "Access granted",   0dh, 0ah, "$"
AccessDeniedStr  db "Access denied",    0dh, 0ah, "$"
TooLongStr       db "Too long input. Samyj umnij chto li? Prosto tak ne perepolnish. Dumaj dalshe", 0dh, 0ah, "$"
end Start
