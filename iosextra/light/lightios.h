#ifndef LIGHTIOS_H_INCLUDE
#define LIGHTIOS_H_INCLUDE

#include <easytype.h>
#include <stdio.h>
#include <stdlib.h> 
#include <ios.h>

#define LED_IOCTL_MODE 0x01

#define RGB_IOCTL_SETR 0x00
#define RGB_IOCTL_SETG 0x01
#define RGB_IOCTL_SETB 0x02
#define RGB_IOCTL_MODE 0x03

typedef struct __HLED
{
	HIOS p;
	INT32 status; 
}_HLED;

typedef struct __HRGB
{
	_HLED* r;
	_HLED* g;
	_HLED* b;
}_HRGB;


_HLED* led_open(CHAR* mode);
INT32 led_ioctl(_HLED* h, INT32 req, VOID* val);
INT32 led_write(_HLED* h,const VOID* v, UINT32 sz, UINT32 n);
VOID led_close(_HLED* h);

_HRGB* rgb_open(CHAR* mode);
INT32 rgb_ioctl(_HRGB* h, INT32 req, VOID* val);
INT32 rgb_write(_HRGB* h,const VOID* v, UINT32 sz, UINT32 n);
VOID rgb_close(_HRGB* h);

#endif

