[BITS 64]

section .text
	global _start

_start:
		call entry
	entry:
		pop r14
		mov r10, [rel payload_off]
		mov r11, (entry - _start)
		add r10, r11
		sub r14, r10

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

		; Calling mprotect to make encrypted sections writable (we need to modify them in order to decrypt !)
		mov rdi, r14
		mov rsi, [rel size]
		mov r12, [rel enc_off]
		add r12, r14
		sub r12, r14
		add rsi, r12
		mov rdx, 7
		mov rax, 10
		syscall

		; Decrypting the encrypted sections
		mov rax, r14
		mov r10, [rel enc_off]
		add rax, r10
		mov rcx, [rel size]
		mov rdx, [rel key]
		add rcx, rax
		jmp decrypt

	decrypt:
		xor byte[rax], dl
		ror rdx, 8
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
		mov r15, r14
		mov r10, [rel enc_off]
		add r15, r10
		jmp r15

align 8
	debug			db "Once",0x0a,0
	debug_len		equ $ - debug
	msg				db "....WOODY....",0x0a,0
	msg_len			equ $ - msg
	payload_off		dq 0x1111111111111111
	key				dq 0x2222222222222222
	enc_off			dq 0x3333333333333333
	size			dq 0x4444444444444444




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
