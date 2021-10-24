[BITS 64]

section .text
	global _start

_start:
		; Saving the registers' initial state
		push rax
		push rdi
		push rsi
		push rdx
		push rcx

		; Displaying the "... Woody ..." message
		mov rax, 1
		mov rdi, 1
		lea rsi, [rel msg]
		mov rdx, msg_len
		syscall

		; Calling mprotect to make encrypted sections writable
		mov rdi, [rel alignedaddr]
		mov rsi, [rel size]

		mov r11, [rel startaddr]
		sub r11, rdi
		add rsi, r11

		mov rdx, 7
		mov rax, 10
		syscall

		; Decrypting the encrypted sections
		mov rax, [rel startaddr]
		mov rcx, [rel size]
		mov rdx, [rel key]
		add rcx, rax
		jmp decrypt

	decrypt:
		xor byte[rax], dl
		inc rax
		cmp rax, rcx
		jne decrypt

		; Restoring the registers to their initial state
		pop rcx
		pop rdx
		pop rsi
		pop rdi
		pop rax

		; Returning to entry point
		mov r15, [rel retaddr]
		jmp r15

align 8
	debug		db "Once",0x0a,0
	debug_len	equ $ - debug
	msg			db "... Woody...",0x0a,0
	msg_len		equ $ - msg
	retaddr		dq 0x1111111111111111
	key			dq 0x2222222222222222
	startaddr	dq 0x3333333333333333
	size		dq 0x4444444444444444
	alignedaddr	dq 0x5555555555555555




;		push rax
;		push rdi
;		push rsi
;		push rdx
;		mov rax, 1
;		mov rdi, 1
;		lea rsi, [rel debug]
;		mov rdx, debug_len
;		syscall
;		pop rdx
;		pop rsi
;		pop rdi
;		pop rax
