#include "prvios.h"

VOID _pwm_set(_HPWM* h)
{
	h->per = (1.0 / (FLOAT64)h->fq);
	h->per -= IOS_OFFSET_SLEEP;
	
	if ( h->duty == 0 )
		h->don = 0.0;
	else
		h->don = ((h->per * h->duty) / 256.0) * 1000000.0;
}

VOID* _pwm(VOID* s)
{
	fflush(stdout);
	
	THREAD_START(s,_HPWM*,h);
	
	_pin_write(h->hp,&h->em,1,1);
		
	FLOAT64 offset;
	FLOAT64 stt = ios_clock_get();
	BYTE w = h->em;
	
	h->elapse = 0;
	
	while (1)
	{
		THREAD_REQUEST();
		
		if ( h->duty > 0 )
		{
			offset = ios_clock_get();
				w = !w;
				//printf("on->%d\n",w);
				_pin_write(h->hp,&w,1,1);
			offset = ios_clock_get() - offset;
			usleep(h->don - offset * 1000000.0);
		}
		else
			w = !w;
		
		if ( h->duty < 255 )
		{
			offset = ios_clock_get();
				w = !w;
				//printf("off->%d\n",w);
			    _pin_write(h->hp,&w,1,1);
			offset = ios_clock_get() - offset;
			usleep( (h->per * 1000000.0 - h->don) - offset * 1000000.0);
		}
		else
			w = !w;
		
		h->elapse = ios_clock_get() - stt;
		if (h->ent && ( h->remt < (ios_clock_get() - stt) ) ) break;
	}
	
	THREAD_ONEXIT

	_pin_write(h->hp,&h->em,1,1);
	
	THREAD_END(NULL);
}

VOID* _rwm(VOID* s)
{	
	THREAD_START(s,_HPWM*,h);
	
	FLOAT64 pet,dot,ent,stt;
	BYTE sync = 0;
	
	h->fq = 0;
	h->duty = 0;
	
	stt = ios_clock_get();
	h->elapse = 0;
	
	while (1)
	{
		THREAD_REQUEST();
		
		for (; !sync; _pin_read(h->hp,&sync,1,1) );
		
		dot = ios_clock_get();
			for (; sync; _pin_read(h->hp,&sync,1,1) );
		pet = ios_clock_get();	
			for (; !sync; _pin_read(h->hp,&sync,1,1) );
		ent = ios_clock_get();
		
		if ( h->fq == 0 )
		{
			h->fq = (UINT32)( 1.0 / ( ent - dot ) );
			h->duty = (UINT32)( (256.0 * (pet - dot)) / (ent -dot) );
		}
		else
		{
			h->fq += (UINT32)( 1.0 / ( ent - dot ) );
			h->duty += (UINT32)( (256.0 * (pet - dot)) / (ent -dot) );
			h->fq >>= 1;
			h->duty >>= 1;
		}
		
		h->elapse = ios_clock_get() - stt;
		if (h->ent && ( h->remt < (ios_clock_get() - stt) ) ) break;
	}
	
	THREAD_ONEXIT
	
	THREAD_END(NULL);
}

_HPWM* _pwm_open(CHAR* m)
{	
	_HPWM* h = malloc(sizeof(_HPWM));
	
	h->d = (*m == 'r') ? 0 : 1;
	
	if ( h->d )
		h->t = thr_new(_pwm,0,0,0);
	else
		h->t = thr_new(_rwm,0,0,0);
	
	if ( !h->t )  { free(h); return NULL;}
	
	h->hp = NULL;
	h->p = 0;
	h->em = 0;
	h->fq = 100;
	h->duty = 0;
	h->tout = 1000;
	h->ent = TRUE;
	h->remt = (FLOAT64)(h->tout)/1000.0;
	_pwm_set(h);
	
	return h;
}

///0 pause 1 start 2 resume
INT32 _pwm_write(_HPWM* h, const VOID* v, UINT32 sz, UINT32 n)
{
	if ( sz < 1 ) return 0;
	
	BYTE* val = (BYTE*) v;
	
	if ( h->p ) 
	{
		if (*val)
			ard_send(CMD_PWM_ENABLE,h->p,0,0);
		else
			ard_send(CMD_PWM_DISABLE,h->p,0,0);
			
		return 1;
	}
	
	if ( !*val )
		thr_requestwait(h->t);
	else if ( *val == 1 )
		thr_run(h->t,h);
	else if ( *val == 2 )
		thr_resume(h->t);
	else 
		return 0;
	
	return 1;
}

INT32 _pwm_read(_HPWM* h, VOID* v, UINT32 sz, UINT32 n)
{
	if ( sz != 4 ) return 0;
	if (h->d != 0) return 0;
	if (h->p == 0) return 0;
	
	while (	h->duty == 0 ) ios_usleep(50);
	
	UINT32* p = (UINT32*) v;
	*p++ = h->fq;
	
	if ( n > 1 )
		*p = h->duty;
	
	return 1;
}

INT32 _pwm_ioctl(_HPWM* h, INT32 req, VOID* s)
{
	UINT32* v = (UINT32*) s;
	FLOAT64* f;
	INT32 ret;
	
	switch (req)
	{
		case IOS_IOCTL_PWM_WAIT:
			if ( !h->t || h->p != 0 ) return IOS_ERR_PWM;
			thr_waitthr(h->t);
		break;
		
		case IOS_IOCTL_PWM_SET:
			if ( *v >= IOS_P00 && *v <= IOS_P17)
			{
				if ( h->hp ) {_pin_close(h->hp); h->hp = NULL;}
				if ( !(h->hp = _pin_open( (h->d) ? "w" : "r" )) ) { return IOS_ERR_OPEN; }
				if ( (ret = _pin_ioctl(h->hp,IOS_IOCTL_PIN_SET,s)) ) {_pin_close(h->hp); h->hp = NULL; return ret;}
			}
			else if ( *v >= IOS_D00 && *v <= IOS_A05)
			{
				if ( h->p != 0 ) ard_send(CMD_PWM_DISABLE,h->p,0,0);
				h->p = *v;
			}
			else
				return IOS_ERR_PIN;
		break;
		
		case IOS_IOCTL_PWM_SET_DUTY:
			if (h->d != 1) return IOS_ERR_IOCTL;
			if ( *v > 255 ) 
				h->duty = 255;
			else
				h->duty = *v;
			if ( h->p )
				ard_send(CMD_PWM_DT_SET,h->p,(BYTE)h->duty,0);
			else
				_pwm_set(h);
		break;
		
		case IOS_IOCTL_PWM_SET_FQ:
			if (h->d != 1) return IOS_ERR_IOCTL;
			if ( *v < 1) return IOS_ERR_PWM;
			h->fq = *v;
			if ( h->p )
			{
				ard_send(CMD_PWM_FQ_SET,h->p,(BYTE)h->fq,(BYTE)(h->fq >> 8));
				ard_send(CMD_PWM_DT_SET,h->p,(BYTE)h->duty,0);
			}
			else
				_pwm_set(h);
		break;
		
		case IOS_IOCTL_PWM_SET_TOUT:
			if ( *v < 0) return IOS_ERR_PWM;
			h->tout = *v;
			h->ent = (h->tout == 0) ? FALSE : TRUE;
			if (h->ent) h->remt = (FLOAT64)(h->tout)/1000.0;
			if ( h->p )
				ard_send(CMD_PWM_TOUT_SET,h->p,(BYTE)h->tout,(BYTE)(h->tout >> 8));
		break;
		
		case IOS_IOCTL_PWM_END_MODE:
			if (h->d != 1 || h->p != 0 ) return IOS_ERR_IOCTL;
			h->em = (*v) ? 1 : 0;
		break;
		
		case IOS_IOCTL_PWM_READ_RESET:
			if (h->d != 0 || h->p != 0) return IOS_ERR_IOCTL;
			thr_requestwait(h->t);
			ios_usleep(1050);
			h->duty = 0;
			h->fq = 0;
			thr_resume(h->t);
		break;
		
		case IOS_IOCTL_PWM_ELAPSE:
			f = (FLOAT64*)s;
			*f = h->elapse;
		break;
		
		default:
		return IOS_ERR_IOCTL;
	}
	
	return 0;
}

VOID _pwm_close(_HPWM* h)
{
	if (h->t)
	{
		thr_stop(h->t,IOS_WAITTHR,1);
		thr_free(h->t);
	}
	
	_pin_close(h->hp);
	
	free(h);
}
