#ifndef MAIN_H_INCLUDE
#define MAIN_H_INCLUDE

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




#endif

