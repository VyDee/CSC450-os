#include "io.h"
#include "idt.h"

/**
 * For the framebuffer,
 *
 * example 0x4128
 * 41: ascii of character
 * 2: foreground color
 * 8: background color
 * hence it is defined as follows.
 *
 * @TODO split them into separate header file.
 */

/* colors */
#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

/* framebuffer mm io */
char* fb = (char *) 0x000B8000;
char* dataSpaceStart = (char*) 0x00300000;

/**
 * display character c on the position i with color fg and bg.
 *
 * @param i the position, 0 for the first line, first col. 16 for the second line.
 * @param c the displayed character
 * @param fg foreground color
 * @param bg background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4 | (bg & 0x0F));
}

/* io ports for cursors */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/**
 * move the cursor to the given position
 *
 * @param short pos: 16 bit totally
 *      - first 8 bit: the row
 *      - last 8 bit: the colomn
 */
void fb_move_cursor(unsigned short pos) 
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}

void fb_write_string(int offset, char* s, int length)
{
    int i;
    for(i = 0; i < length; i = i + 1)
    {
        fb_write_cell(offset + i*2, s[i], FB_GREEN, FB_DARK_GREY);
    }
}

void fb_write_char(int offset, unsigned char s, unsigned char fg, unsigned char bg)
{
    fb[offset] = s;
    fb[offset+1] = ((fg & 0x0F) << 4 | (bg & 0x0F));
}

void fb_clear()
{
    for(int i = 0; i < 80*25; i++)
    {
        fb_write_cell(i*2, ' ', FB_BLACK, FB_BLACK);
    }
}

/*char pointer*/

char* originalByte = (char*) 0x00300000;
char* endByte = (char*) 0x1F00000;
char* memoryStartPointer;
// void unmarkAllMemory()
// {
//     while (originalByte < endByte)
//     {

//     }
// }
void memoryInitialize()
{
    memoryStartPointer = originalByte + ((endByte - originalByte) / 9);
}

void setBitToOne (int bitPos,int count)
{
    char* target =  originalByte + (bitPos/8);
    *target |= (1 << (7 - (bitPos % 8)));
    // possible recursion stack overflow?? might have to rewrite as iterative in order for large allocations to work
    if (count > 1)
    {
        setBitToOne (bitPos + 1 , count - 1);
    }
}

char* getMem(int numByte)
{
    int count = 0;
    int bitCount = 0;
    char* curByte = originalByte;
    while (curByte < memoryStartPointer)
    {

        for (int i = 7; i >= 0; i--)

        {
            bitCount++;
            if ((*curByte >> i) & 1)
            {
                count = 0;
            }
            else
            {
                count++;
                if(count == numByte)
                {
                    setBitToOne(bitCount - count, count);
                    return memoryStartPointer + (bitCount - count);
                }
            }
        }
        curByte++;
    }
    return (char*)0xCAFEBABE;
}


int main() 
{
    /* display the cursor in the second line */
    //int cursorPos = 0x0050 * 10;
    
    //fb_move_cursor(0x0800);
    interrupts_install_idt();
    memoryInitialize();
    //char* s = dataSpaceStart;
    char* s = getMem(10); //malloc 5 * sizeof(char) -> give me 5 bytes
    s[0] = 'h';
    s[1]= 'e';
    s[2] = 'l';
    s[3] = 'l';
    s[4] = 'o';
    
    fb_clear();
    fb_write_string(80*2*0, s,10);
   

    return 0;
}