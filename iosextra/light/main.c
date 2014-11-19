#include "main.h"

_HLED* led_open(CHAR* mode)
{
	if ( *mode != 'w' ) return NULL;
	
	_HLED* ld = malloc(sizeof(ld));
	
	ld->p = ios_open("ios/pwm",mode);
	ld->status = 0;
	return ld;
}

INT32 led_ioctl(_HLED* h, INT32 req, VOID* val)
{	
	switch (req)
	{
		case IOS_IOCTL_PIN_SET:
			if ( h->status != 0 ) 
			{
				ios_close(h->p);
				h->p = ios_open("ios/pwm","w");
			}
			ios_ioctl(h->p,IOS_IOCTL_PWM_SET,val);
			INT32 v = 500;
			ios_ioctl(h->p,IOS_IOCTL_PWM_SET_FQ,&v);
			v = 0;
			ios_ioctl(h->p,IOS_IOCTL_PWM_END_MODE,&v);
			ios_ioctl(h->p,IOS_IOCTL_PWM_SET_DUTY,&v);
			ios_ioctl(h->p,IOS_IOCTL_PWM_SET_TOUT,&v);
			BYTE b = 1;
			ios_write(h->p,&b,1,1);
			h->status = 1;
		break;
		
		case IOS_IOCTL_PWM_SET_FQ:
			return ios_ioctl(h->p,req,val);
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 led_write(_HLED* h,const VOID* v, UINT32 sz, UINT32 n)
{
	if (sz < 1) return 0;
	ios_ioctl(h->p,IOS_IOCTL_PWM_SET_DUTY,v);
	return 1;
}


VOID led_close(_HLED* h)
{
	ios_close(h->p);
	free(h);
}

/*
int main(int argc,char* argv[])
{
    return 0;   
}
*/
