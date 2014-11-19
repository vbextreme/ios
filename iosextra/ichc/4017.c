#include "ichc.h"

_H4017* i4017_open(CHAR* mode)
{
	if ( *mode != 'w' ) return NULL;
	
	_H4017* h = malloc(sizeof(_H4017));
	
	h->clk = ios_open("ios/pin",mode);
	h->e = ios_open("ios/pin",mode);
	h->rs = ios_open("ios/pin",mode);
	h->reverse = FALSE;
	h->count = 255;
	h->autor = 0;
	h->enable = FALSE;
	h->c = 0;
	return h;
}

INT32 i4017_ioctl(_H4017* h, INT32 req, VOID* val)
{	
	INT32* v = (INT32*)val;
	BYTE b;
	
	switch (req)
	{
		case H4017_MODE:
			if (!*v)
			{
				HIOS tmp = h->clk;
				h->clk = h->e;
				h->e = tmp;
				h->reverse = TRUE;
				h->c = 1;
			}
			
			ios_write(h->clk,&h->c,1,1);
			b = !h->reverse;
			ios_write(h->e,&b,1,1);
		break;
		
		case H4017_PIN_CLOCK:
			return ios_ioctl(h->clk,req,val);
		case H4017_PIN_ENABLE:
			return ios_ioctl(h->clk,req,val);
		case H4017_PIN_RESET:
			return ios_ioctl(h->clk,req,val);
		
		case H4017_ENABLE:
			b = (*v) ? h->reverse : !h->reverse;
			h->enable = *v;
			ios_write(h->e,&b,1,1);
		break;
		
		case H4017_RESET:
			if ( h->autor ) h->count = h->autor;
			b = 1;
			ios_write(h->rs,&b,1,1);
			ios_nsleep(300);
			b = 0;
			ios_write(h->rs,&b,1,1);
		break;
		
		case H4017_AUTORESET:
			h->autor = *v;
		break;
		
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 i4017_write(_H4017* h,const VOID* v, UINT32 sz, UINT32 n)
{
	if (sz < 1) return 0;
	
	if ( h->enable )
	{
		BYTE v = !h->c;
		ios_write(h->clk,&v,1,1);
		ios_nsleep(500);
		ios_write(h->e,&h->c,1,1);
		if ( h->autor )
		{
			++h->count;
			if ( h->count >= h->autor )
				i4017_ioctl(h,H4017_RESET,NULL);
		}
	}
	
	return 1;
}


VOID i4017_close(_H4017* h)
{
	ios_close(h->clk);
	ios_close(h->rs);
	ios_close(h->e);
	free(h);
}
