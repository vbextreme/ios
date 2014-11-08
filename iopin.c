#include "prvios.h"

/// ///// ///
/// U3LED ///
/// ///// ///

_HU3L* _u3l_open(CHAR* mode)
{
	if (*mode == 'r') return NULL;
	_HU3L* h = malloc(sizeof(_HU3L));
	
	CHAR pport[MAX_INPATH];
	sprintf(pport,"%s%s",FILE_ULED,"trigger");
	if ( (h->f = open(pport,O_WRONLY)) == -1 )  
	{
		free(h);
		return NULL;
	}
	return h;
}

INT32 _u3l_ioctl(_HU3L* h, INT32 req, VOID* val)
{
	INT32* p = (INT32*) val;
	CHAR pport[MAX_INPATH];
	INT32 f;
	
	switch (req)
	{
		case IOS_IOCTL_U3L_MMC:
			if ( write(h->f,"mmc1",4) < 0 ) {return IOS_ERR_IOCTL;}
			fsync(h->f);
		break;
		case IOS_IOCTL_U3L_SD:
			if ( write(h->f,"mmc0",4) < 0 ) {return IOS_ERR_IOCTL;}
			fsync(h->f);
		break;
		case IOS_IOCTL_U3L_TIMER:
			if ( write(h->f,"timer",5) < 0 ) {return IOS_ERR_IOCTL;}
			fsync(h->f);
		break;
		case IOS_IOCTL_U3L_ONESHOT:
			if ( write(h->f,"oneshot",7) < 0 ) {return IOS_ERR_IOCTL;}
			fsync(h->f);
			sprintf(pport,"%s%s",FILE_ULED,"shot");
			if ( (f = open(pport,O_WRONLY)) < 0 )  {return IOS_ERR_OPEN;}
			if ( write(f,"1",1) < 0 ) {close(f);return IOS_ERR_IOCTL;}
			fsync(f);
			close(f);
		break;
		case IOS_IOCTL_U3L_GPIO:
			if ( write(h->f,"gpio",4) < 0 ) {return IOS_ERR_IOCTL;}
			fsync(h->f);
		break;
		case IOS_IOCTL_U3L_TIMER_ON:	
			sprintf(pport,"%s%s",FILE_ULED,"delay_on");
			if ( (f = open(pport,O_CREAT | O_WRONLY | O_TRUNC)) < 0 )  {return IOS_ERR_OPEN;}
			sprintf(pport,"%d",*p);
			if ( write(f,pport,strlen(pport)) < 0 ) {close(f);return IOS_ERR_IOCTL;}
			fsync(f);
			close(f);
		break;
		case IOS_IOCTL_U3L_TIMER_OFF:	
			sprintf(pport,"%s%s",FILE_ULED,"delay_off");
			if ( (f = open(pport,O_CREAT | O_WRONLY | O_TRUNC)) < 0 )  {return IOS_ERR_OPEN;}
			sprintf(pport,"%d",*p);
			if ( write(f,pport,strlen(pport)) < 0 ) {close(f);return IOS_ERR_IOCTL;}
			fsync(f);
			close(f);
		break;
		
		default:
			return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 _u3l_write(_HU3L* h,const BYTE* v, UINT32 sz)
{
	if (sz != 1) return 0;
	
	if ( write(h->f,(*v) ? "default-on" : "none",(*v) ? 10 : 4) < 0 ) {return 0;}
	
	fsync(h->f);
	return 1;
}

VOID _u3l_close(_HU3L* h)
{
	if ( h->f != -1 ) close(h->f);
	free(h);
}



/// /// ///
/// PIN ///
/// /// ///

_HIO* _pin_open(CHAR* mode)
{
	_HIO* h = malloc(sizeof(_HIO));
	
	h->p = 0;
	h->d = (*mode == 'r') ? IOS_DIR_IN : IOS_DIR_OUT;
	h->a = FALSE;
	h->f = -1;
	
	return h;
}

INT32 _pin_ioctl(_HIO* h, INT32 req, VOID* val)
{
	INT32* p = (INT32*) val;
	CHAR pport[MAX_INPATH];
	
	switch (req)
	{
		case IOS_IOCTL_PIN_SET:
			
			if ( h->p ) {close(h->f); h->p = 0;}
			
			if ( *p >= IOS_P00 && *p <= IOS_P17)
			{
				h->a = FALSE;
				if ( !ios_exported(*p) ) return IOS_ERR_EXPORT;
				if ( ios_dir_write(*p,h->d) ) return IOS_ERR_DIR;
				sprintf(pport,"%s%d%s",FILE_PIN,*p,FILE_VALUE);
				if ( (h->f = open(pport,(h->d) ? O_WRONLY : O_RDONLY)) == -1 )  {return IOS_ERR_OPEN;}
				h->p = *p;
			}
			else if ( *p >= IOS_D00 && *p <= IOS_A05)
			{
				h->a = TRUE;
				h->f = -1;
				h->p = *p - 1000;
				if ( !ard_send(CMD_PIN_DIR,h->p,h->d,0) ) return IOS_ERR_DIR;
			}
			else
				return IOS_ERR_PIN;
		break;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 _pin_write(_HIO* h,const BYTE* v, UINT32 sz)
{
	if (sz != 1) return 0;
	if (h->d != 1) return 0;
	
	if ( h->a )
	{
		return ard_send(CMD_PIN_WRITE,(BYTE)h->p,*v,0);
	}
	
	if ( write(h->f,(*v) ? "1" : "0",1) < 0 ) {return 0;}
	fsync(h->f);
	return 1;
}

INT32 _pin_read(_HIO* h, BYTE* v, UINT32 sz)
{
	if ( sz != 2 ) return 0;
	if (h->d != 0) return 0;	
	BYTE vr[4];
	
	if ( h->a )
	{
		UINT32 ret = ard_send(CMD_PIN_READ,(BYTE)h->p,0,0);
		if ( !ret ) return 0;
		ret = ard_recv(vr);
		if ( !ret ) return 0;
		if ( vr[0] != CMD_PIN_READ ) return 0;
		*v++ = vr[3];
		*v   = vr[2];
		return (INT32)ret;
	}
	
	lseek(h->f,0,SEEK_SET);
	if ( read(h->f,vr,1) < 0 ) {return 0;}
	
	*v++ = vr[0] - '0';
	*v++ = 0;
	
	return 1;
}

VOID _pin_close(_HIO* h)
{
	if ( h->f != -1 ) close(h->f);
	free(h);
}

/// //// ///
/// PORT ///
/// //// ///

_HPORT* _port_open(CHAR* mode)
{	
	_HPORT* h = malloc(sizeof(_HPORT));
		
	int i;
	for (i = 0; i < 8; i++)
		h->p[i] = NULL;
	
	h->d = (*mode == 'r') ? IOS_DIR_IN : IOS_DIR_OUT;
	
	return h;
}

INT32 _port_ioctl(_HPORT* h, INT32 req, VOID* val)
{
	INT32* p = (INT32*) val;
	INT32 ret,id = -1 ;
	
	switch (req)
	{
		case IOS_IOCTL_PORT_SET_0:
		case IOS_IOCTL_PORT_SET_1:
		case IOS_IOCTL_PORT_SET_2:
		case IOS_IOCTL_PORT_SET_3:
		case IOS_IOCTL_PORT_SET_4:
		case IOS_IOCTL_PORT_SET_5:
		case IOS_IOCTL_PORT_SET_6:
		case IOS_IOCTL_PORT_SET_7: 
			id = req; 
		break;
		
		case IOS_IOCTL_PORT_SET_A:
		
			for ( id = 0; id < 8; ++id, ++p )
			{
				if ( *p != 0 )
				{
					if ( h->p[id] ) {_pin_close(h->p[id]); h->p[id] = NULL;}
					if ( !(h->p[id] = _pin_open((h->d) ? "w" : "r")) ) { return IOS_ERR_OPEN; }
					if ( (ret = _pin_ioctl(h->p[id],IOS_IOCTL_PIN_SET,p)) ) {_pin_close(h->p[id]); h->p[id] = NULL; return ret;}
				}
			}
		
		return 0;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	if ( id == -1 ) return IOS_ERR_IOCTL;
	if ( h->p[id] ) {_pin_close(h->p[id]); h->p[id] = NULL;}
	if ( !(h->p[id] = _pin_open((h->d) ? "w" : "r")) ) { return IOS_ERR_OPEN; }
	if ( (ret = _pin_ioctl(h->p[id],IOS_IOCTL_PIN_SET,val)) ) {_pin_close(h->p[id]); h->p[id] = NULL; return ret;}
	
	return 0;
}

INT32 _port_write(_HPORT* h,const BYTE* v, UINT32 sz)
{
	if (sz != 1) return 0;
	if (h->d != 1) return 0;
	
	int i;
	BYTE bit;
	
	for (i = 0; i < 8; i++)
	{
		if ( h->p[i] ) 
		{
			bit = (*v >> i) & 0x01;
			if ( !_pin_write(h->p[i],&bit,1) ) return 0;
		}
	}
	return 1;
}

INT32 _port_read(_HPORT* h, BYTE* v, UINT32 sz)
{
	if (sz != 1) return 0;
	if (h->d != 0) return 0;
	
	*v = 0;
	
	int i;
	UINT16 r;
	for (i = 0; i < 8; i++) 
	{
		if ( h->p[i] ) 
		{
			if ( !_pin_read(h->p[i],(BYTE*)&r,2) ) return 0;
			*v |= ( r << i );
		}
	}
	return 1;
}

VOID _port_close(_HPORT* h)
{
	int i;
	for (i = 0; i < 8; i++)
		if (h->p[i]) _pin_close(h->p[i]);
	free(h);
}

/// ///// ///
/// PULSE ///
/// ///// ///

_HPULSE* _pulse_open(CHAR* mode)
{
	_HPULSE* h = malloc(sizeof(_HPULSE));
	h->d = (*mode == 'r') ? IOS_DIR_IN : IOS_DIR_OUT;
	h->us = 50;
	h->p = NULL;
	h->s = 0;
	h->ck = ios_uclock_get();
	h->a = 0;
	h->mr = 0;
	
	return h;
}

INT32 _pulse_ioctl(_HPULSE* h, INT32 req, VOID* val)
{
	UINT32* p = (UINT32*) val;
	INT32 ret;
	
	switch (req)
	{
		case IOS_IOCTL_PULSE_SET:
			if ( *p >= IOS_P00 && *p <= IOS_P17)
			{
				h->a = 0;
				if ( h->p ) _pin_close(h->p);
				if ( !(h->p = _pin_open((h->d) ? "w" : "r")) ) { return IOS_ERR_OPEN; }
				if ( (ret = _pin_ioctl(h->p,IOS_IOCTL_PIN_SET,val)) ) {_pin_close(h->p); h->p = NULL; return ret;}
				if ( h->d && !_pin_write(h->p,&h->s,1) ) {_pin_close(h->p); h->p = NULL; return IOS_ERR_WRITE;}
			}
			else if ( *p >= IOS_D00 && *p <= IOS_A05)
			{
				h->a = *p - 1000;
			}
			else
				return IOS_ERR_PIN;
		break;
		
		case IOS_IOCTL_PULSE_MODE:
			if ( h->d )
			{
				h->s = (*p) ? 0 : 1;
				if ( h->a )
					ard_send(CMD_PIN_WRITE,h->a,!h->s,0);
				else
				{
					h->s = !h->s;
					if ( !_pin_write(h->p,&h->s,1) ) return IOS_ERR_WRITE;
				}
			}
			else
				h->s = (*p) ? 1 : 0;
				
			h->ck = ios_uclock_get();
		break;
		
		case IOS_IOCTL_PULSE_US:
			if ( !h->d ) return IOS_ERR_DIR;
			h->us = *p;
		break;
		
		case IOS_IOCTL_PULSE_MODERET:
			if ( h->d ) return IOS_ERR_DIR;
			h->mr = *p;
		break;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

INT32 _pulse_write(_HPULSE* h,const BYTE* v, UINT32 sz)
{
	if (sz != 1) return 0;
	if (h->d != 1) return 0;
	if (*v != 1) return 0;
	
	if ( h->a )
	{
		BYTE c = ( h->s ) ? CMD_PULSEU_OUT : CMD_PULSED_OUT;
		ard_send(c, h->a, (BYTE)h->us, (BYTE)(h->us >> 8));
		return 1;
	}
	
	UINT32 nck = ios_uclock_get();
	if ( nck - h->ck < h->us ) ios_usleep( nck - h->ck );
	
	h->s = !h->s;
	if ( !_pin_write(h->p,&h->s,1) ) {return 0;}
	ios_usleep( h->us );
	
	h->s = !h->s;
	if ( !_pin_write(h->p,&h->s,1) ) {return 0;}
	
	h->ck = ios_uclock_get();
	return 1;
}

INT32 _pulse_read(_HPULSE* h, BYTE* v, UINT32 sz)
{
	if ( sz != 8 ) return 0;
	if (h->d != 0 ) return 0;
	
	FLOAT64 *res =(FLOAT64*) v;
	
	if ( h->a )
	{
		BYTE c = ( h->mr ) ? CMD_PULSEC_IN : CMD_PULSEF_IN;
		ard_send(c, h->a, h->s, 0);
		BYTE r[4];
		ard_recv(r);
		if ( r[0] != c ) return IOS_ERR_READ;
		UINT32 clk = r[1] | (UINT32)(r[2] << 8) | (UINT32)(r[3] << 16);
		*res = (FLOAT64)(clk);
		*res /= 1000000.0;
		if ( !h->mr )
			*res = 1.0 / *res;
		return 1;
	}
	
	
	UINT16 r;
	FLOAT64 clks;
	
	for (_pin_read(h->p,(BYTE*)&r,2); r == h->s; _pin_read(h->p,(BYTE*)&r,2) );
	for (_pin_read(h->p,(BYTE*)&r,2); r != h->s; _pin_read(h->p,(BYTE*)&r,2) );
	
	clks = ios_clock_get();
	for (_pin_read(h->p,(BYTE*)&r,2); r == h->s; _pin_read(h->p,(BYTE*)&r,2) );
	if ( h->mr )
	{
		*res = (ios_clock_get() - clks);
		return 1;
	}
	
	for (_pin_read(h->p,(BYTE*)&r,2); r != h->s; _pin_read(h->p,(BYTE*)&r,2) );
	*res = 1.0 / (ios_clock_get() - clks);
	return 1;
}

VOID _pulse_close(_HPULSE* h)
{
	if (h->p) _pin_close(h->p);
	free(h);
}

/// ///////// ///
/// INTERRUPT ///
/// ///////// ///

VOID* _rint(VOID* s)
{
	THREAD_START(s,_HINT*,h);
	
	BYTE set = h->intset;
	UINT32 rd = !set;
	
	while (1)
	{
		THREAD_REQUEST();
		
		do
		{
			for (_pin_read(h->p,(BYTE*)&rd,2); set != rd; _pin_read(h->p,(BYTE*)&rd,2) )
				ios_usleep(h->bou);
			ios_usleep(h->bou);
			_pin_read(h->p,(BYTE*)&rd,2);
		}while ( set != rd);
		
		if ( h->mode == IOS_INT_CBK )
		{
			if (h->cbk) h->cbk(h->param);
		}
		else
		{
			if (fork() == 0 )
			{
				execl("/bin/sh", "sh", "-c", h->ph, (CHAR*)0);
				return 0;
			}
		}
		
		for ( _pin_read(h->p,(BYTE*)&rd,2); set == rd; _pin_read(h->p,(BYTE*)&rd,2) )
				ios_usleep(h->bou);
	}
	
	THREAD_ONEXIT
	
	THREAD_END(NULL);
}

_HINT* _int_open(CHAR* m)
{
	if (*m == 'w') return NULL;
	
	_HINT* h = malloc(sizeof(_HINT));
	h->p = NULL;
	h->mode = IOS_INT_CBK;
	h->ph[0] = '\0';
	h->cbk = NULL;
	h->param = NULL;
	h->intset = 1;
	h->bou = 300;
	h->t = thr_new(_rint,0,0,0);
	
	return h;
}

INT32 _int_ioctl(_HINT* h, INT32 req, VOID* val)
{
	UINT32* p = (UINT32*) val;
	INT32 ret;
	
	switch (req)
	{
		case IOS_IOCTL_INT_SET:
			if ( h->p ) _pin_close(h->p);
			if ( !(h->p = _pin_open("r") )) { return IOS_ERR_OPEN; }
			if ( (ret = _pin_ioctl(h->p,IOS_IOCTL_PIN_SET,val)) ) {_pin_close(h->p); h->p = NULL; return ret;}
		break;
		
		case IOS_IOCTL_INT_MODE:
			h->mode = *p;
		break;
		
		case IOS_IOCTL_INT_MODE_SET:
			if ( h->mode == IOS_INT_EXECL )
				strcpy(h->ph,(CHAR*)val);
			else if ( h->mode == IOS_INT_CBK )
				h->cbk = (IOSINT)val;
			else
				return IOS_ERR_IOCTL;
		break;
		
		case IOS_IOCTL_INT_PARAM:
			h->param = val;
		break;
		
		case IOS_IOCTL_INT_STAT:
			h->intset = *p;
		break;
		
		case IOS_IOCTL_INT_BOUNCE:
			h->bou = *p;
		break;
		
		case IOS_IOCTL_INT_WAIT:
			if ( !h->t ) return IOS_ERR_IOCTL;
			thr_waitthr(h->t);
		break;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

///0 pause 1 start 2 resume
INT32 _int_write(_HINT* h,const BYTE* v, UINT32 sz)
{
	if ( sz != 1 ) return 0;

	if ( *v == 0 )
		thr_requestwait(h->t);
	else if ( *v == 1 )
		thr_run(h->t,h);
	else if ( *v == 2 )
		thr_resume(h->t);
	else 
		return 0;
	
	return 1;
}

VOID _int_close(_HINT* h)
{
	if (h->t)
	{
		thr_stop(h->t,IOS_WAITTHR,1);
		thr_free(h->t);
	}
	
	_pin_close(h->p);
	
	free(h);
}
