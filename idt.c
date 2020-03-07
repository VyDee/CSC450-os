#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "kmain.h"
#include "pic.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256
#define INTERRUPTS_KEYBOARD 33
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9

static unsigned char keyboard_scan_code_to_ascii[256] =
{
	0x0, 0x0, '1', '2', '3', '4', '5', '6',		// 0 - 7
	'7', '8', '9', '0', '-', '=', 0x0, 0x0,		// 8 - 15
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',		// 16 - 23
	'o', 'p', '[', ']', '\n', 0x0, 'a', 's',	// 24 - 31
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',		// 32 - 39
	'\'', '`', 0x0, '\\', 'z', 'x', 'c', 'v',	// 40 - 47
	'b', 'n', 'm', ',', '.', '/', 0x0, '*',		// 48 - 55
	0x0, ' ', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,		// 56 - 63
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, '7',		// 64 - 71
	'8', '9', '-', '4', '5', '6', '+', '1',		// 72 - 79
	'2', '3', '0', '.'				// 80 - 83
};

struct IDTDescriptor idt_descriptors[INTERRUPTS_KEYBOARD];
struct IDT idt;

void interrupts_init_descriptor(int index, unsigned int address)
{
    idt_descriptors[index].offset_high = (address >> 16) & 0xFFFFFF; //offset high 16bbits
    idt_descriptors[index].offset_low = (address & 0xFFFF);
    idt_descriptors[index].segment_selector = 0x08; //the second (code) segment selector in GDT: one segment is 64gb
    idt_descriptors[index].reserved = 0x00; //Reserved
    idt_descriptors[index].types_and_attributes = (0x01 << 7)| //P
                                                  (0x00 << 6)| (0x00 <<5)| //DPL
                                                  (0xe); //1110 = e (set gate to 1 = 32 bits)
}

void interrupts_install_idt()
{
    interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (unsigned int) interrupt_handler_32);
    idt.address = (int) &idt_descriptors;
    idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT;
    interrupts_load_idt((int) &idt);

    pic_remap(0x20, 0x28);
}

void interrupt_handler(__attribute__((unused)) struct cpu_state cpu, __attribute__((unused)) struct stack_state stack, unsigned int interrupt)
{
    if (interrupt == 32)
    {
        unsigned char scan_code = keyboard_read_scan_code();
        unsigned char ascii = keyboard_scan_code_to_ascii[scan_code];
        fb_write_char(80*2*5,ascii,FB_DARK_GREY,FB_LIGHT_BLUE);
        pic_acknowledge(interrupt);
    }
}
