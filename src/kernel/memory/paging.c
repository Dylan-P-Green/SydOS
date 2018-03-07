#include <main.h>
#include <tools.h>
#include <kprint.h>
#include <string.h>
#include <kernel/paging.h>
#include <kernel/pmm.h>
#include <arch/i386/kernel/cpuid.h>
#include <arch/i386/kernel/interrupts.h>

// http://www.rohitab.com/discuss/topic/31139-tutorial-paging-memory-mapping-with-a-recursive-page-directory/
// https://forum.osdev.org/viewtopic.php?f=15&t=19387
// https://medium.com/@connorstack/recursive-page-tables-ad1e03b20a85

extern uint32_t KERNEL_PAGE_DIRECTORY;
extern uint32_t KERNEL_PAGE_TEMP;
extern bool PAE_ENABLED;

static uint32_t paging_calculate_table(page_t virtAddr) {
    return virtAddr / PAGE_SIZE_4M;
}

static uint32_t paging_calculate_entry(page_t virtAddr) {
    return virtAddr % PAGE_SIZE_4M / PAGE_SIZE_4K;
}

static uint32_t paging_pae_calculate_directory(page_t virtAddr) {
    return virtAddr / PAGE_SIZE_1G;
}

static uint32_t paging_pae_calculate_table(page_t virtAddr) {
    return (virtAddr - (paging_pae_calculate_directory(virtAddr) * PAGE_SIZE_1G)) / PAGE_SIZE_2M;
}

static uint32_t paging_pae_calculate_entry(page_t virtAddr) {
    return virtAddr % PAGE_SIZE_2M / PAGE_SIZE_4K;
}

void paging_change_directory(uintptr_t directoryPhysicalAddr) {
    // Tell CPU the directory and enable paging.
    asm volatile ("mov %%eax, %%cr3": :"a"(directoryPhysicalAddr)); 
    asm volatile ("mov %cr0, %eax");
    asm volatile ("orl $0x80000000, %eax");
    asm volatile ("mov %eax, %cr0");
}

void paging_flush_tlb() {
    // Flush TLB.
    asm volatile ("mov %cr3, %eax");
    asm volatile ("mov %eax, %cr3");
}

void paging_flush_tlb_address(uintptr_t address) {
    // Flush specified address in TLB.
    asm volatile ("invlpg (%0)" : : "b"(address) : "memory");
}

void paging_map_virtual_to_phys(uint32_t *directory, page_t virt, page_t phys) {
    // Calculate table and entry of virtual address.
    uint32_t tableIndex = paging_calculate_table(virt);
    uint32_t entryIndex = paging_calculate_entry(virt);

    // Get address of table from directory.
    // If there isn't one, create one.
    // Pages will never be located at 0x0, so its safe to assume a value of 0 = no table defined.   
    if (MASK_PAGE_4K(directory[tableIndex]) == 0) {
        directory[tableIndex] = pmm_pop_frame() | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;
        paging_flush_tlb();
    }
    uint32_t *table = (uint32_t*)(PAGE_TABLE_ADDRESS_START + (tableIndex * PAGE_SIZE_4K));

    // Add address to table.
    table[entryIndex] = phys | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;
    
    // Flush TLB
    paging_flush_tlb_address(virt);
}

void paging_map_kernel_virtual_to_phys(page_t virt, page_t phys) {
    paging_map_virtual_to_phys((uint32_t*)0xFFFFF000, virt, phys);
}

void paging_unmap_virtual(uintptr_t *directory, page_t virt) {
    // Calculate table and entry of virtual address.
    uint32_t tableIndex = paging_calculate_table(virt);
    uint32_t entryIndex = paging_calculate_entry(virt);

    // Get pointer to table.
    page_t *table = (page_t*)(PAGE_TABLE_ADDRESS_START + (tableIndex * PAGE_SIZE_4K));

    // Clear address from table.
    table[entryIndex] = 0;
    paging_flush_tlb_address(virt);
}

void paging_unmap_kernel_virtual(page_t virt) {
    paging_unmap_virtual((uint32_t*)memInfo.kernelPageDirectory, virt);
}

void paging_map_region(page_t *directory, page_t startAddress, page_t endAddress, bool kernel, bool writeable) {
    // Ensure addresses are on 4KB boundaries.
    startAddress = MASK_PAGE_4K(startAddress);
    endAddress = MASK_PAGE_4K(endAddress);

    // Map space.
    
}

static void paging_pagefault_handler() {
    

    page_t addr;
    asm volatile ("mov %%cr2, %0" : "=r"(addr));

    panic("Page fault at 0x%X!\n", addr);
}

void paging_late() {
    // Get pointer to the early-paging page table for 0x0.
    uint32_t *earlyPageTableLow = (uint32_t*)(PAGE_TABLE_ADDRESS_START);

    // Pop a new page frame for page directory, and map it to 0x0 in the current virtual space.
    page_t pageDirectoryAddr = pmm_pop_frame();
    earlyPageTableLow[0] = pageDirectoryAddr | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;

    // Get pointer to page directory.
    page_t *pageDirectory = (page_t*)0x0;
    memset(pageDirectory, 0, PAGE_SIZE_4K);

    // Create the first page table for the kernel, and map it to 0x1000 in the current virtual space.
    page_t pageKernelTableAddr = pmm_pop_frame();
    earlyPageTableLow[1] = pageKernelTableAddr | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;
    paging_flush_tlb();

    // Get pointer to page table.
    page_t *pageKernelTable = (page_t*)0x1000;
    memset(pageKernelTable, 0, PAGE_SIZE_4K);

    // Add the table to the new directory.
    pageDirectory[paging_calculate_table(memInfo.kernelVirtualOffset)] = pageKernelTableAddr | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;

    // Map low memory and kernel to higher-half virtual space.
    uint32_t offset = 0;
    for (page_t page = 0; page <= memInfo.pageFrameStackEnd - memInfo.kernelVirtualOffset; page += PAGE_SIZE_4K) {
        // Have we reached the need to create another table?
        if (page > 0 && page % PAGE_SIZE_4M == 0) { 
            // Create another table and map to 0x1000 in the current virtual space.
            pageKernelTableAddr = pmm_pop_frame();
            earlyPageTableLow[1] = pageKernelTableAddr | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;
            paging_flush_tlb();

            // Zero out new page table.
            memset(pageKernelTable, 0, PAGE_SIZE_4K);

            // Increase offset and add the table to our new directory.
            offset++;
            pageDirectory[paging_calculate_table(memInfo.kernelVirtualOffset) + offset] = pageKernelTableAddr;
        }

        // Add page to table.
        pageKernelTable[(page / PAGE_SIZE_4K) - (offset * 1024)] = page | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;
    }

    // Recursively map page directory to last entry.
    pageDirectory[PAGE_DIRECTORY_SIZE - 1] = pageDirectoryAddr | PAGING_PAGE_READWRITE | PAGING_PAGE_PRESENT;

    // Change to use our new page directory.
    paging_change_directory(pageDirectoryAddr);
}

void paging_init() {
    // Store info from early boot.
    memInfo.kernelPageDirectory = KERNEL_PAGE_DIRECTORY;
    memInfo.kernelPageTemp = KERNEL_PAGE_TEMP;
    memInfo.paeEnabled = PAE_ENABLED;

    // Wire up page fault handler.
    interrupts_isr_install_handler(ISR_EXCEPTION_PAGE_FAULT, paging_pagefault_handler);

    // Is PAE enabled?
    if (memInfo.paeEnabled) {
        kprintf("PAE enabled!\n");

        // Create page directory pointer table (PDPT).
        uint64_t *pageDirectoryPointerTablePreBoot = ((uint64_t*)memInfo.kernelPageDirectory) + 4;
        uint64_t *pageDirectoryPointerTable = (uint64_t*)memInfo.kernelPageDirectory;
        for (uint32_t d = 0; d < PAGE_PAE_PDPT_SIZE; d++)
            pageDirectoryPointerTable[d] = pmm_pop_frame() | PAGING_PAGE_PRESENT;

        // Create temporary page directory and map it to 0x0.
        uint64_t *pageTempTable = (uint64_t*)memInfo.kernelPageTemp;

       // pageDirectoryPointerTablePreBoot[0] =  | PAGING_PAGE_PRESENT;
        paging_flush_tlb();

        // Map kernel to 0x0. When the new PDPT is loaded, it will actually be at 0xC0000000.
        for (page_t i = memInfo.kernelVirtualOffset; i <= memInfo.pageFrameStackEnd; i+= PAGE_SIZE_4K)
            paging_map_virtual_to_phys(0x0, i, i - memInfo.kernelVirtualOffset);
    }
    else { // No PAE, using standard paging.
        paging_late();
    }

    // Pop physical page for test.
    page_t page = pmm_pop_frame();
    kprintf("Popped page 0x%X for test...\n", page);
    
    // Map physical page to 0x1000 for testing.
    paging_map_kernel_virtual_to_phys(0x1000, page);

    // Test memory at location.
    kprintf("Testing memory at virtual address 0x1000...\n");
    uintptr_t *testPage = (uintptr_t*)0x1000;
    for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++)
        testPage[i] = i;

    bool pass = true;
    for (uint32_t i = 0; i < PAGE_TABLE_SIZE; i++)
        if (testPage[i] != i) {
            pass = false;
            break;
        }
    kprintf("Test %s!\n", pass ? "passed" : "failed");
    if (!pass)
        panic("Memory test of virtual address 0x1000 failed.\n");

    // Unmap virtual address and return page to stack.
    kprintf("Unmapping 0x1000 and pushing page 0x%X back to stack...\n", page);
    paging_unmap_kernel_virtual(0x1000);
    pmm_push_frame(page);

    kprintf("Paging initialized!\n");
}
