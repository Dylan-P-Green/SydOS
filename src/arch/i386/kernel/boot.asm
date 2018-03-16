; 32-bit code.
[bits 32]

; Multiboot constants.
MULTIBOOT_PAGE_ALIGN    equ 0x00000001
MULTIBOOT_MEMORY_INFO   equ 0x00000002
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS  equ  MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

; Multiboot section.
section .multiboot
align 4
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

; Allocate stack
section .bss
	align 16
stack_bottom:
	resb 16384
stack_top:

; Start function.
section .init
global _start
_start:
	; Disable interrupts.
	cli

	; Push Multiboot info structure and magic number.
	push ebx
	push eax

	; Call the early kernel main function.
	; This sets up early boot paging, etc.
	extern kernel_main_early
	call kernel_main_early

	; Pop Multiboot info off.
	pop eax
	pop ebx

	; Jump to higher half.
	lea edx, [_start_higherhalf]
	jmp edx

; Higher-half of our start function.
section .text
_start_higherhalf:
	; Point stack pointer to top.
	mov esp, stack_top
 
	; Ensure stack is 16-bit aligned.
    and esp, -16

	; Push Multiboot info structure and magic number.
	push ebx

	; Call the kernel main function!
	extern kernel_main
	call kernel_main
 
	; Disable interrupts and loop forever.
	cli
.hang:	hlt
	jmp .hang