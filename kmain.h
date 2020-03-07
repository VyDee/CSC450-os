#ifndef INCLUDE_KMAIN_H
#define INCLUDE_KMAIN_H

void fb_write_string(int offset, char* s, int length);
void fb_write_char(int offset, unsigned char s, unsigned char fg, unsigned char bg);
void fb_clear();

#endif 