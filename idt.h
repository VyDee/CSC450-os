#ifndef _INCLUDE_INTERRUPT
#define _INCLUDE_INTERRUPT

struct IDT {
    unsigned short size; //size will be 2KB which will fit in a short
    unsigned int address;
}__attribute__((packed));

struct IDTDescriptor{
    unsigned short offset_high;
    unsigned char types_and_attributes;
    unsigned char reserved;
    unsigned short segment_selector;
    unsigned short offset_low;
}__attribute__((packed));

struct cpu_state {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    //unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    } __attribute__((packed));

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

void interrupts_install_idt();

//Wrappers around ASM
void interrupts_load_idt(unsigned int idt_address);
void interrupt_handler_32();

void interrupts_init_descriptor(int index, unsigned int address);
void interrupts_install_idt();
void interrupt_handler(struct cpu_state cpu, struct stack_state stack, unsigned int interrupt);



#endif 