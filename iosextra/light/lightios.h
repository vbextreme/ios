#ifndef LIGHTIOS_H_INCLUDE
#define LIGHTIOS_H_INCLUDE

#include <easytype.h>
#include <stdio.h>
#include <stdlib.h> 
#include <ios.h>


typedef struct __HLED
{
	HIOS p;
	INT32 status; 
}_HLED;

_HLED* led_open(CHAR* mode);
INT32 led_ioctl(_HLED* h, INT32 req, VOID* val);
INT32 led_write(_HLED* h,const VOID* v, UINT32 sz, UINT32 n);
VOID led_close(_HLED* h);


#endif

