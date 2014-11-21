#include "lightios.h"

_HRGB* rgb_open(CHAR* mode)
{
	if ( *mode != 'w' ) return NULL;
	
	_HRGB* ld = malloc(sizeof(_HRGB));
	
	ld->r = led_open("w");
	ld->g = led_open("w");
	ld->b = led_open("w");
	
	ld->status = 0;
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
			if ( *((INT32*)(val)) ) 
				h->status = 2;
			else
				h->status = 1;
		break;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 rgb_write(_HRGB* h,const VOID* v, UINT32 sz, UINT32 n)
{
	if (sz < 3) return 0;
	
	if ( h->status == 2 )
	{
		const BYTE* c = (const BYTE*) v;
		BYTE s;
		s = 255 - *c++;
		led_ioctl(h->r,IOS_IOCTL_PWM_SET_DUTY,&s);
		s = 255 - *c++;
		led_ioctl(h->g,IOS_IOCTL_PWM_SET_DUTY,&s);
		s = 255 - *c++;
		led_ioctl(h->b,IOS_IOCTL_PWM_SET_DUTY,&s);
	}
	else
	{
		const BYTE* c = (const BYTE*) v;
		BYTE s = *c++;
		led_ioctl(h->r,IOS_IOCTL_PWM_SET_DUTY,&s);
		s = *c++;
		led_ioctl(h->g,IOS_IOCTL_PWM_SET_DUTY,&s);
		s = *c;
		led_ioctl(h->b,IOS_IOCTL_PWM_SET_DUTY,&s);
	}
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
