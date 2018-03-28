[bits 64]
section .text

; Exception ISRs
global _isr0
global _isr1
global _isr2
global _isr3
global _isr4
global _isr5
global _isr6
global _isr7
global _isr8
global _isr9
global _isr10
global _isr11
global _isr12
global _isr13
global _isr14
global _isr15
global _isr16
global _isr17
global _isr18
global _isr19
global _isr20
global _isr21
global _isr22
global _isr23
global _isr24
global _isr25
global _isr26
global _isr27
global _isr28
global _isr29
global _isr30
global _isr31

;  0: Divide By Zero Exception
_isr0:
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
_isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

;  2: Non Maskable Interrupt Exception
_isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

;  3: Int 3 Exception
_isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

;  4: INTO Exception
_isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

;  5: Out of Bounds Exception
_isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

;  6: Invalid Opcode Exception
_isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

;  7: Coprocessor Not Available Exception
_isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

;  8: Double Fault Exception (With Error Code!)
_isr8:
    cli
    push byte 8
    jmp isr_common_stub

;  9: Coprocessor Segment Overrun Exception
_isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
_isr10:
    cli
    push byte 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
_isr11:
    cli
    push byte 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
_isr12:
    cli
    push byte 12
    jmp isr_common_stub

; 13: General Protection Fault Exception (With Error Code!)
_isr13:
    cli
    push byte 13
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
_isr14:
    cli
    push byte 14
    jmp isr_common_stub

; 15: Reserved Exception
_isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: Floating Point Exception
_isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment Check Exception
_isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: Machine Check Exception
_isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: Reserved
_isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Reserved
_isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Reserved
_isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: Reserved
_isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved
_isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved
_isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved
_isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved
_isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved
_isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved
_isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: Reserved
_isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: Reserved
_isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: Reserved
_isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

; IRQ ISRs
global _irq0
global _irq1
global _irq2
global _irq3
global _irq4
global _irq5
global _irq6
global _irq7
global _irq8
global _irq9
global _irq10
global _irq11
global _irq12
global _irq13
global _irq14
global _irq15

; 32: IRQ0
_irq0:
    cli
    push byte 0
    push byte 32
    jmp isr_common_stub

; 33: IRQ1
_irq1:
    cli
    push byte 0
    push byte 33
    jmp isr_common_stub

; 34: IRQ2
_irq2:
    cli
    push byte 0
    push byte 34
    jmp isr_common_stub

; 35: IRQ3
_irq3:
    cli
    push byte 0
    push byte 35
    jmp isr_common_stub

; 36: IRQ4
_irq4:
    cli
    push byte 0
    push byte 36
    jmp isr_common_stub

; 37: IRQ5
_irq5:
    cli
    push byte 0
    push byte 37
    jmp isr_common_stub

; 38: IRQ6
_irq6:
    cli
    push byte 0
    push byte 38
    jmp isr_common_stub

; 39: IRQ7
_irq7:
    cli
    push byte 0
    push byte 39
    jmp isr_common_stub

; 40: IRQ8
_irq8:
    cli
    push byte 0
    push byte 40
    jmp isr_common_stub

; 41: IRQ9
_irq9:
    cli
    push byte 0
    push byte 41
    jmp isr_common_stub

; 42: IRQ10
_irq10:
    cli
    push byte 0
    push byte 42
    jmp isr_common_stub

; 43: IRQ11
_irq11:
    cli
    push byte 0
    push byte 43
    jmp isr_common_stub

; 44: IRQ12
_irq12:
    cli
    push byte 0
    push byte 44
    jmp isr_common_stub

; 45: IRQ13
_irq13:
    cli
    push byte 0
    push byte 45
    jmp isr_common_stub

; 46: IRQ14
_irq14:
    cli
    push byte 0
    push byte 46
    jmp isr_common_stub

; 47: IRQ15
_irq15:
    cli
    push byte 0
    push byte 47
    jmp isr_common_stub

; ISR common stub. This calls the handler defined in interrupts.c.
extern interrupts_isr_handler
isr_common_stub:
    ; The processor has already pushed SS, RSP, RFLAGS, CS, and RIP to the stack.
    ; The interrupt-specific handler also pushed the interrupt number, and an empty error code if needed.
    
    ; Push general registers (RAX, RCX, RDX, RBX, RBP, RSI, and RDI) to stack.
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi

    ; Push x64 registers (R15, R14, R13, R12, R11, R10, R9, and R8) to stack.
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8

    ; Push segments to stack.
    ; DS and ES cannot be directly pushed, so we must copy them to RAX first.
    mov rax, ds
    push rax
    mov rax, es
    push rax
    push fs
    push gs

    ; Set up kernel segments.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler.
    mov rdi, rsp
    call interrupts_isr_handler

global _isr_exit
_isr_exit:
    ; Restore segments.
    ; DS and ES cannot be directly restored, so we must copy them to RAX first.
    pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    ; Restore x64 registers (R8, R9, R10, R11, R12, R13, R14, R15) to stack.
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    ; Restore general registers (RDI, RSI, RBP, RBX, RDX, RCX, and RAX).
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    ; Move past the error code and interrupt number and continue execution.
    add rsp, 16
    iretq

global _isr_empty
_isr_empty:
    iretq
