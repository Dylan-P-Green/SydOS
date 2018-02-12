#include "main.h"
#include "tools.h"
#include "kernel/gdt.h"
#include "kernel/nmi.h"
#include "kernel/idt.h"
#include "kernel/interrupts.h"
#include "kernel/pit.h"
#include "kernel/memory.h"
#include "kernel/paging.h"
#include "kernel/cpuid.h"
#include "driver/vga.h"
#include "driver/floppy.h"
#include "driver/serial.h"

#include "logging.h"

/**
 * Kernel's ending address in RAM
 */
extern uint32_t kernel_end;
/**
 * Kernel's starting address in RAM
 */
extern uint32_t kernel_base;

/**
 * Function in enable_a20.asm to enable the A20 line.
 * This should be moved to a header file :(
 */
extern void _enable_A20();
extern void _enable_protected_mode();
extern void _disable_protected_mode();
extern int _get_protected_mode();

/**
 * The main function for the kernel, called from boot.asm
 */
void kernel_main(void) {
	
	asm volatile("cli");
	

	vga_disable_cursor();
	
	serial_initialize();
	const char* data = "If you're reading this, serial works.\n";
	for (size_t i = 0; i < strlen(data); i++) {
		serial_write(data[i]);
	}

	vga_initialize();
	vga_setcolor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
	vga_writes("   _____           _  ____   _____ \n");
	vga_writes("  / ____|         | |/ __ \\ / ____|\n");
	vga_writes(" | (___  _   _  __| | |  | | (___  \n");
	vga_writes("  \\___ \\| | | |/ _` | |  | |\\___ \\ \n");
	vga_writes("  ____) | |_| | (_| | |__| |____) |\n");
	vga_writes(" |_____/ \\__, |\\__,_|\\____/|_____/ \n");
	vga_writes("          __/ |                    \n");
	vga_writes("         |___/                     \n");
	vga_writes("Copyright (c) Sydney Erickson 2017 - 2018\n");
	
	// -------------------------------------------------------------------------
	// MEMORY RELATED STUFF
	vga_setcolor(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);

	vga_writes("Checking A20 line...\n");
	int AX;
	asm( "movl $0, %0"
   		: "=a" (AX)
    );
    if (AX == 0) {
    	vga_writes("Enabling A20 line...\n");
		_enable_A20();
    } else if (AX == 1) {
    	vga_writes("A20 line already enabled.\n");
    } else {
    	vga_writes("A20 line detection returned invalid result!\n");
    }

	char kernbase[32], kernend[32];
	itoa((uint32_t)&kernel_base, kernbase, 16);
	itoa((uint32_t)&kernel_end, kernend, 16);
	log("Kernel start: 0x");
	log(kernbase);
	log(" | Kernel end: 0x");
	log(kernend);
	log("\n");

	memory_init((uint32_t)&kernel_end);
	memory_print_out();

    // TODO: detect CPUID support before calling for it
	vga_setcolor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);

	// Print CPUID info.
	cpuid_print_capabilities();
	vga_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	// -------------------------------------------------------------------------

	log("Initializing GDT...\n");
	gdt_init();

	log("Initializing IDT...\n");
	idt_init();

	log("Initializing interrupts...\n");
	interrupts_init();

	log("Enabling NMI...\n");
	NMI_enable();

	//_enable_protected_mode();
	protected_mode_land();
	
}

void protected_mode_land() {
	vga_setcolor(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREEN);
	log("Kernel has entered protected mode.\n");
	vga_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    // Enable interrupts.
	asm volatile("sti");
    vga_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    log("INTERRUPTS ARE ENABLED\n");
    vga_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	log("Setting up PIT...\n");
    pit_init();

    log("Initializing paging...\n");
    paging_initialize();

	// Floppy test.
	floppy_detect();
	log("Initialize floppy drives...\n");
	floppy_init();

    vga_enable_cursor();

	log ("Sleeping for 5 seconds...\n");
	sleep(5000);
	log ("Current uptime: ");
	char* temp;
	utoa(pit_ticks(), temp, 10);
	log(temp);
	log(" milliseconds.\n");

    vga_setcolor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
	vga_writes("root@sydos ~: ");
	serial_writes("root@sydos ~: ");
	vga_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    // Ring serial terminal.
	serial_write('\a');

	for(;;) {
		char* input[80];
		int i = 0;
		char c = serial_read();

		if (c == '\r' || c == '\n') {
			vga_setcolor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
			vga_writes("\nroot@sydos ~: ");
			serial_writes("\nroot@sydos ~: ");
			vga_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
		} else {
			input[i] = &c;
			i++;
			vga_putchar(c);
			serial_write(c);
		}
	}
}