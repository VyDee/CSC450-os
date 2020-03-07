#include "pic.h"
#include "io.h"

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xA0

/* The PIC interrupts have been remapped */
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT   PIC2_START_INTERRUPT + 7

#define PIC_MASTER_ADDR 0x20
#define PIC_SLAVE_ADDR 0xA0

#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_DATA 0xA1

#define PIC_ACK     0x20

#define ICW1_ICW4    0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE    0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4    0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL    0x08        /* Level triggered (edge) mode */
#define ICW1_INIT    0x10        /* Initialization - required! */

#define ICW4_8086    0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO    0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE    0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER    0x0C        /* Buffered mode/master */
#define ICW4_SFNM    0x10        /* Special fully nested (not) */

void pic_acknowledge(unsigned int interrupt)
{
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return;
    }
    
    if (interrupt < PIC2_START_INTERRUPT) {
        outb(PIC1_PORT_A, PIC_ACK);
    } else {
        outb(PIC2_PORT_A, PIC_ACK);
    }
}

void pic_remap(int offset1, int offset2)
{
    outb(PIC_MASTER_ADDR, ICW1_INIT + ICW1_ICW4);    // starts the initialization sequence (in cascade mode)
    outb(PIC_SLAVE_ADDR, ICW1_INIT + ICW1_ICW4);
    outb(PIC_MASTER_DATA, offset1);                // ICW2: Master PIC vector offset
    outb(PIC_SLAVE_DATA, offset2);                // ICW2: Slave PIC vector offset
    outb(PIC_MASTER_DATA, 4);                    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC_SLAVE_DATA, 2);                    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    
    outb(PIC_MASTER_DATA, ICW4_8086);
    outb(PIC_SLAVE_DATA, ICW4_8086);
    
    // Setup Interrupt Mask Register (IMR)
    outb(PIC_MASTER_DATA, 0xFD); // 1111 1101 - Enable IRQ 1 only (keyboard).
    outb(PIC_SLAVE_DATA, 0xFF);
    
    asm("sti"); // Enable interrupts.
}