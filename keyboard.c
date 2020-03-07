#include "keyboard.h"
#include "io.h"

#define KEYBOARD_DATAPORT 0x60

/** read_scan_code:
 *  Reads a scan code from the keyboard
 *
 *  @return The scan code (NOT an ASCII character!)
 * 
 */
unsigned char keyboard_read_scan_code(void)
{
	return inb(KEYBOARD_DATAPORT);
}