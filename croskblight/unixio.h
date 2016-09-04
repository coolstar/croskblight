#include <fltkernel.h>

#define inb __inbyte
#define outb(value, port) __outbyte(port, value)
#define fprintf(a,b) 
#define printf(a,b)