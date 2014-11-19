#ifndef ICHC_H_INCLUDE
#define ICHC_H_INCLUDE

#include <easytype.h>
#include <stdio.h>
#include <stdlib.h> 
#include <ios.h>

#define H4017_PIN_CLOCK  0x0
#define H4017_PIN_ENABLE 0x1
#define H4017_PIN_RESET  0x2
#define H4017_MODE       0x3
#define H4017_RESET      0x4
#define H4017_ENABLE     0x5
#define H4017_AUTORESET  0x6

typedef struct __H4017
{
	HIOS clk;
	HIOS e;
	HIOS rs;
	BOOL reverse;
	BOOL enable;
	BYTE count;
	BYTE autor;
	BYTE c;
}_H4017;

_H4017* i4017_open(CHAR* mode);
INT32 i4017_ioctl(_H4017* h, INT32 req, VOID* val);
INT32 i4017_write(_H4017* h,const VOID* v, UINT32 sz, UINT32 n);
VOID i4017_close(_H4017* h);

#endif

