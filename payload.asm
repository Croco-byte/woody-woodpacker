section .text
	global _start

_start:
		push rax
		push rdi
		push rsi
		push rdx
		push rcx

		mov rax, 1
		mov rdi, 1
		lea rsi, [rel msg]
		mov rdx, msg_end - msg
		syscall
		
		pop rcx
		pop rdx
		pop rsi
		pop rdi
		pop rax

		mov r15, 0x1111111111111111
		push r15
		ret

align 8
	msg db "Hello from woody",0x0a,0
	msg_end db 0x0
