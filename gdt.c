#include <stdint.h>
#include <string.h>

// Define the structure of a GDT entry
struct __attribute__((packed)) gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;       // P | DPL | S | Type
    uint8_t granularity;  // (G | D | L | AVL) | (limit_high)
    uint8_t base_high;
};

// Define the structure of a TSS entry
struct __attribute__((packed)) tss_entry {
    uint32_t reserved1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iomap_base;
    char iomap[8192];
};
// struct __attribute__((packed)) tss_entry{
//     uint32_t prev_tss;
//     uint32_t esp0;
//     uint32_t ss0;
//     uint32_t esp1;
//     uint32_t ss1;
//     uint32_t esp2;
//     uint32_t ss2;
//     uint32_t cr3;
//     uint32_t eip;
//     uint32_t eflags;
//     uint32_t eax;
//     uint32_t ecx;
//     uint32_t edx;
//     uint32_t ebx;
//     uint32_t esp;
//     uint32_t ebp;
//     uint32_t esi;
//     uint32_t edi;
//     uint32_t es;
//     uint32_t cs;
//     uint32_t ss;
//     uint32_t ds;
//     uint32_t fs;
//     uint32_t gs;
//     uint32_t ldt;
//     uint16_t trap;
//     uint16_t iomap_base;
// };

struct tss_entry tss;

// Declare the GDT and GDT pointer
// Make room for 128 entries, although we only use the first 3
// Use the aligned attribute to ensure that the GDT is 8-byte aligned
struct gdt_entry gdt[128] __attribute__((aligned(8)));
#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10
#define USER_32BIT_CODE_SEGMENT 0x18
#define USER_DATA_SEGMENT 0x20
#define USER_CODE_SEGMENT 0x28
#define KERNEL_TSS_SEGMENT 0x30


// Function to set a GDT entry
void gdt_set_gate(int num, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity) {
    // Set the base address
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    // Set the limit
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);

    // Set the access flags
    gdt[num].access = access;
}
void load_gdt(void *gdt, unsigned short limit);

void setup_gdt() {

    // Clear the gdt
    memset(&gdt, 0, sizeof(gdt));

    // Null segment
    gdt_set_gate(0, 0, 0, 0, 0);  // 0x00

    // Code segment
    gdt_set_gate(1, 0, 0, 0x9A, 0x20); // 0x08
    // uint8_t access;       // P | DPL | S | Type
    // uint8_t granularity;  // (G | D | L | AVL) | (limit_high)
    // Data segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // 0x10

    // gate 3 is reserved for 32-bit user code segment

    // User Data segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // 0x20

    // User Code segment
    gdt_set_gate(5, 0, 0, 0xFA, 0x20); // 0x28
    // gdt_set_gate(5, 0, 0, 0xFA, 0xE0); // 0x28
    
    memset(&tss, 0, sizeof(tss));
    // ESP is 1MB-64KB = 0x100000-0x10000 = 0xF0000
    tss.rsp0 = 0xF0000;
    tss.iomap_base = (char *)&tss.iomap - (char *)&tss;
 
    // TSS segment      // 0x30
    gdt_set_gate(6, (uint64_t)&tss, sizeof(struct tss_entry), 0xE9, 0x00);

    // Load the GDT
    // load_gdt with gdt and sizeof gdt
    load_gdt(gdt, sizeof(gdt)-1);

    // Load the task register using ax
    asm ("ltr %w0" : : "a" (KERNEL_TSS_SEGMENT));
}
