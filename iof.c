#include "prvios.h"
#include <dlfcn.h>

const BYTE IOS_PIN_LOW = 0;
const BYTE IOS_PIN_HIGH = 1;

const BYTE IOS_PWM_PAUSE = 0;
const BYTE IOS_PWM_START = 1;
const BYTE IOS_PWM_RESUME = 2;

static HSRL _com = NULL;

VOID ard_init()
{
	if ( _com ) return;
	_com = srl_open(FILE_COM,SRL_NONCANONICAL,COM_SPEED,8,0,1,COM_WAIT,COM_CMD);
}

VOID ard_close()
{
	if ( !_com ) return;
	srl_close(_com);
	_com = NULL;
}

UINT32 ard_send(BYTE cmd, BYTE p0, BYTE p1, BYTE p2)
{
	if ( !_com ) return 0;
	UINT32 vs = ((UINT32)(p2)<< 24) | ((UINT32)(p1)<< 16) | ((UINT32)(p0)<< 8) | cmd;
	return srl_write(_com,&vs,4);
}

UINT32 ard_recs(BYTE* cmd, BYTE* p0, BYTE* p1, BYTE* p2)
{
	if ( !_com ) return 0;
	
	BYTE rv[4];
	UINT32 ret = srl_read(_com,rv,4);
	*cmd = rv[0];
	*p0 = rv[1];
	*p1 = rv[2];
	*p2 = rv[3];
	return ret;
}

UINT32 ard_recv(BYTE* cppp)
{
	if ( !_com ) return 0;
	
	return srl_read(_com,cppp,4);
}

/// //////////////// ///
/// SUPPORT FUNCTION ///
/// //////////////// ///

CHAR* _nextp(CHAR* s)
{
	while( *s && *s++ != '/' );
	return s;
}

CHAR* _getp(CHAR* d, INT32 sz, CHAR* s)
{
	while( *s && sz-- && *s != '/' ) *d++ = *s++;
	*d = '\0';
	return (*s) ? s + 1 : s;
}


/// ///////////// ///
/// BASE FUNCTION ///
/// ///////////// ///

FLOAT64 ios_clock_get()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

UINT32 ios_mclock_get()
{
	return (UINT32)(ios_clock_get() * 1000.0);
}

UINT32 ios_uclock_get()
{
	return (UINT32)(ios_clock_get() * 1000000.0);
}

UINT32 ios_nclock_get()
{
	return (UINT32)(ios_clock_get() * 1000000000.0);
}

VOID ios_msleep(UINT32 ms)
{
	thr_msleep(ms);
}

VOID ios_usleep(UINT32 us)
{
	thr_nsleep(us * 1000);
}

VOID ios_nsleep(UINT32 nano)
{
	thr_nsleep(nano);
}

BOOL ios_exported(UINT32 pin)
{
	if ( pin < IOS_P_MIN || pin > IOS_P_MAX ) {return FALSE;}
	
	CHAR pport[80];
	sprintf(pport,"%s%d%s",FILE_PIN,pin,FILE_DIRECTION);
	
	INT32 f;
	f = open(pport,O_WRONLY);
	if ( f < 0 ) return FALSE;
	
	close(f);
	return TRUE;
}

INT32 ios_export(UINT32 pin, UINT32 mode)
{
	if ( pin < IOS_P_MIN || pin > IOS_P_MAX ) {return IOS_ERR_PIN;}
	
	INT32 f;
	if ( mode == IOS_UNEXPORT )
		f = open(FILE_UNEXPORT,O_WRONLY);
	else
		f = open(FILE_EXPORT,O_WRONLY);
	if ( f < 0 ) {return IOS_ERR_OPEN;}
	
	char pinname[8];
	sprintf(pinname,"%d",pin);
	if ( write(f,pinname,3) < 0 ) {close(f);return IOS_ERR_WRITE;}
	
	close(f);
	return 0;
}

INT32 ios_exportall(UINT32 mode)
{
	INT32 i,ret;
	for ( i = IOS_P00; i <= IOS_P17; ++i)
		if ( (ret = ios_export(i,mode)) ) return ret;
	return 0;
}

INT32 ios_dir_write(UINT32 pin, UINT32 d)
{
	if ( pin < IOS_P_MIN || pin > IOS_P_MAX ) {return IOS_ERR_PIN;}
	
	CHAR pport[80];
	sprintf(pport,"%s%d%s",FILE_PIN,pin,FILE_DIRECTION);
	
	INT32 f;
	f = open(pport,O_WRONLY);
	if ( f < 0 )  {return IOS_ERR_OPEN;}
	
	if ( write(f,(d)?"out":"in",(d)?3:2) < 0 ) {close(f);return IOS_ERR_WRITE;}
	
	close(f);
	return 0;
}

INT32 ios_dir_read(UINT32 pin)
{
	if ( pin < IOS_P_MIN || pin > IOS_P_MAX ) {return IOS_ERR_PIN;}
	
	CHAR pport[80];
	sprintf(pport,"%s%d%s",FILE_PIN,pin,FILE_DIRECTION);
	
	INT32 f;
	f = open(pport,O_RDONLY);
	if ( f == -1 )  {return IOS_ERR_OPEN;}
	
	if ( read(f,pport,4) < 0 ) {close(f);return IOS_ERR_WRITE;}
	
	close(f);
	
	return (pport[0] == 'i') ? 0 : 1;
}

HIOS ios_open(CHAR* t,CHAR* m)
{
	CHAR nl[IOS_LIB_NAME_MAX];
	CHAR nd[IOS_DEV_NAME_MAX];
	
	_HIOS* h = malloc(sizeof(_HIOS));
		if ( !h ) return NULL;
	h->dl = NULL;

	t = _getp(nl,IOS_LIB_NAME_MAX,t);
		if ( !*t ) { free(h); return NULL; }
	t = _getp(nd,IOS_DEV_NAME_MAX,t);
		if ( *t ) { free(h); return NULL; }

	if ( strcmp(nl , "ios") )
	{
		h->m = HM_LDL;
		
		CHAR ph[80];
		strcpy(ph,PATH_DL);
		strcat(ph,"lib");
		strcat(ph,nl);
		strcat(ph,".so");
		
		if ( !(h->dl = dlopen(ph,RTLD_LOCAL)) ) {free(h); return NULL;}
		
		strcpy(ph,nd);
		strcat(ph,"_open");
		_IOSOPEN iop;
		if ( !(iop = (_IOSOPEN) dlsym(h->dl,ph) ) ) { dlclose(h->dl); free(h); return NULL;}
		strcpy(ph,nd);
		strcat(ph,"_write");
		if ( !(h->w = (_IOSWRITE) dlsym(h->dl,ph)) ) { dlclose(h->dl); free(h); return NULL;}
		strcpy(ph,nd);
		strcat(ph,"_read");
		if ( !(h->r = (_IOSREAD) dlsym(h->dl,ph)) ) { dlclose(h->dl); free(h); return NULL;}
		strcpy(ph,nd);
		strcat(ph,"_ioctl");
		if ( !(h->i = (_IOSIOCTL) dlsym(h->dl,ph)) ) { dlclose(h->dl); free(h); return NULL;}
		strcpy(ph,nd);
		strcat(ph,"_close");
		if ( !(h->c = (_IOSCLOSE) dlsym(h->dl,ph)) ) { dlclose(h->dl); free(h); return NULL;}
		
		if ( !(h->p = iop(t,m)) ) { dlclose(h->dl); free(h); return NULL;}
		return h;
	}

	if ( !strcmp(nd,"u3l") )
	{
		h->m = HM_U3L;
		if ( !(h->p = _u3l_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _u3l_write;
		h->r = NULL;
		h->i = (_IOSIOCTL) _u3l_ioctl;
		h->c = (_IOSCLOSE) _u3l_close;
	}
	else if ( !strcmp(nd,"pin") )
	{
		h->m = HM_PIN;
		if ( !(h->p = _pin_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _pin_write;
		h->r = (_IOSREAD) _pin_read;
		h->i = (_IOSIOCTL) _pin_ioctl;
		h->c = (_IOSCLOSE) _pin_close;
	}
	else if ( !strcmp(nd , "port") )
	{
		h->m = HM_PORT;
		if ( !(h->p = _port_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _port_write;
		h->r = (_IOSREAD) _port_read;
		h->i = (_IOSIOCTL) _port_ioctl;
		h->c = (_IOSCLOSE) _port_close;
	}
	else if ( !strcmp(nd , "pulse") )
	{
		h->m = HM_PULSE;
		if ( !(h->p = _pulse_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _pulse_write;
		h->r = (_IOSREAD) _pulse_read;
		h->i = (_IOSIOCTL) _pulse_ioctl;
		h->c = (_IOSCLOSE) _pulse_close;
	}
	else if ( !strcmp(nd , "pwm") )
	{
		h->m = HM_PWM;
		if ( !(h->p = _pwm_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _pwm_write;
		h->r = (_IOSREAD) _pwm_read;
		h->i = (_IOSIOCTL) _pwm_ioctl;
		h->c = (_IOSCLOSE) _pwm_close;
	}
	else if ( !strcmp(nd , "int") )
	{
		h->m = HM_INT;
		if ( !(h->p = _int_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _int_write;
		h->r = NULL;
		h->i = (_IOSIOCTL) _int_ioctl;
		h->c = (_IOSCLOSE) _int_close;
	}
	else if ( !strcmp(nd , "328") )
	{
		h->m = HM_328;
		if ( !(h->p = _328_open(m)) ) {free(h); return NULL;}
		h->w = (_IOSWRITE) _328_write;
		h->r = (_IOSREAD) _328_read;;
		h->i = (_IOSIOCTL) _328_ioctl;
		h->c = (_IOSCLOSE) _328_close;
	}
	else
	{
		free(h);
		h = NULL;
	}
	
	return h;
}

INT32 ios_write(HIOS h,const BYTE* v, UINT32 sz)
{
	if (h && h->w) 
		return h->w(h->p,v,sz);
	return 0;
}

INT32 ios_read(HIOS h, BYTE* v, UINT32 sz)
{
	if (h && h->r) 
		return h->r(h->p,v,sz);
	return 0;
}

INT32 ios_ioctl(HIOS h, INT32 req, VOID* v)
{
	if (h && h->i) 
		return h->i(h->p,req,v);
	return 0;
}

VOID ios_close(HIOS h)
{
	if ( h->c ) 
		h->c(h->p);
	if (h->dl)
		dlclose(h->dl);
	free(h);
}
