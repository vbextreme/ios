#include "lightios.h"

//00RRGGBB

_HRGB* rgb_open(CHAR* mode)
{
	if ( *mode != 'w' ) return NULL;
	
	_HRGB* ld = malloc(sizeof(_HRGB));
	
	ld->r = led_open("w");
	ld->g = led_open("w");
	ld->b = led_open("w");
	
	return ld;
}

INT32 rgb_ioctl(_HRGB* h, INT32 req, VOID* val)
{	
	switch (req)
	{
		case RGB_IOCTL_SETR:
			return led_ioctl(h->r,IOS_IOCTL_PIN_SET,val);
		
		case RGB_IOCTL_SETG:
			return led_ioctl(h->g,IOS_IOCTL_PIN_SET,val);
		
		case RGB_IOCTL_SETB:
			return led_ioctl(h->b,IOS_IOCTL_PIN_SET,val);
		
		case RGB_IOCTL_MODE:
			led_ioctl(h->r,LED_IOCTL_MODE,val);
			led_ioctl(h->g,LED_IOCTL_MODE,val);
			led_ioctl(h->b,LED_IOCTL_MODE,val);
		return 0;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 rgb_write(_HRGB* h,const VOID* v, UINT32 sz, UINT32 n)
{
	if (sz < 3) return 0;
	
	const BYTE* c = (const BYTE*) v;
	
	BYTE s = *c++;
	led_write(h->b,&s,1,1);
	s = *c++;
	led_write(h->g,&s,1,1);
	s = *c;
	led_write(h->r,&s,1,1);
	
	return 1;
}


VOID rgb_close(_HRGB* h)
{
	led_close(h->r);
	led_close(h->g);
	led_close(h->b);
	free(h);
}

/*
int main(int argc,char* argv[])
{
    return 0;   
}
*/
