#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

void pic_acknowledge(unsigned int interrupt);
void pic_remap(int offset1, int offset2);
#endif