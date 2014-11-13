#include "prvios.h"

_H328* _328_open(CHAR* m)
{
	_H328* h = malloc(sizeof(_H328));
	
	h->stwr = 0;
	return h;
}

INT32 _328_write(_H328* h, const VOID* v, UINT32 sz, UINT32 n)
{
	BYTE* val = (BYTE*) v;
	
	if ( h->stwr == IOS_IOCTL_328_PRE_SET )
	{
		if ( sz != 2 ) return 0;
		ard_send(CMD_PWM_AUTO,*val,*(val+1),0);
		return 1;
	}
	else if ( h->stwr == IOS_IOCTL_328_DIR )
	{
		if ( sz != 2 ) return 0;
		ard_send(CMD_PIN_WRITE,*val,*(val+1),0);
		return 1;
	}
	else if ( h->stwr == IOS_IOCTL_328_MEM_STA )
	{
		while ( n-- )
		{
			ard_send(CMD_MEM_WRITE,*val,0,0);
			val += sz;
		}
	}
	
	return 0;
}

INT32 _328_read(_H328* h,VOID* v, UINT32 sz, UINT32 n)
{
	BYTE cmd[4];
	UINT32* i = (UINT32*) v;
	BYTE* val = (BYTE*) v;
	
	if ( h->stwr == IOS_IOCTL_328_DIR )
	{
		if ( sz != 4 ) return 0;
		ard_send(CMD_PIN_READ,*i,0,0);
		ard_recv(cmd);
		if ( cmd[0] != CMD_PIN_READ || cmd[1] != *i ) return 0;
		if ( *i > 13 )
			*i = cmd[2] | ((UINT32)(cmd[3]) << 8);
		else
			*i = cmd[2];
		return 1;
	}
	else if ( h->stwr == IOS_IOCTL_328_MEM_STA )
	{
		while( n-- )
		{
			ard_send(CMD_MEM_READ,0,0,0);
			ard_recv(cmd);
			if ( cmd[0] != CMD_MEM_READ ) return 0;
			*val = cmd[1];
			val += sz;
		}
	}
	
	return 0;
}

INT32 _328_ioctl(_H328* h, INT32 req, VOID* val)
{
	BYTE cmd[4];
	BYTE* b;
	INT32* i;
	
	switch (req)
	{
		case IOS_IOCTL_328_ECHO:
		case IOS_IOCTL_328_WAKE:
			ard_send(CMD_ECHO,1,2,3);
			ard_recv(cmd);
			if ( cmd[0] != CMD_ECHO || cmd[1] != 1 || cmd[2] != 2 || cmd[3] != 3 ) return IOS_ERR_IOCTL;
		break;
		
		case IOS_IOCTL_328_SLEEP:
			ard_send(CMD_SLEEP,0,0,0);
		break;
		
		case IOS_IOCTL_328_OPZ:
			b = (BYTE*) val;
			ard_send(CMD_OPZ,*b,0,0);
		break;
		
		case IOS_IOCTL_328_MAXTO:
			i = (INT32*)val;
			ard_send(CMD_MAXTO,(BYTE)(*i),(BYTE)(*i >> 8),0);
		break;
		
		case IOS_IOCTL_328_PRE_SET:
			b = (BYTE*)val;
			ard_send(CMD_PRE_SET,*b,*(b+1),0);
			h->stwr = IOS_IOCTL_328_PRE_SET;
		break;
		
		case IOS_IOCTL_328_MEM_STA:
			i = (INT32*)val;
			ard_send(CMD_MEM_STA,(BYTE)(*i),(BYTE)(*i >> 8),0);
			h->stwr = IOS_IOCTL_328_MEM_STA;
		break;
		
		case IOS_IOCTL_328_NOP:
			ard_send(CMD_NOP,0,0,0);
		break;
		
		case IOS_IOCTL_328_OPPC:
			i = (INT32*)val;
			ard_send(CMD_OPPC,(BYTE)(*i),(BYTE)(*i >> 8),0);
		break;
		
		case IOS_IOCTL_328_DIR:
			b = (BYTE*)val;
			ard_send(CMD_PIN_DIR,*b,*(b+1),0);
			h->stwr = IOS_IOCTL_328_DIR;
		break;
		
		case IOS_IOCTL_328_OVDT:
			ard_send(CMD_OVDT,0,0,0);
			ard_recv(cmd);
			if ( cmd[0] != CMD_OVDT ) return IOS_ERR_IOCTL;
			i = (INT32*) val;
			*i = 0;
			*i = cmd[1] | ((INT32)(cmd[2]) << 8) | ((INT32)(cmd[3]) << 16);
		break;
		
		case IOS_IOCTL_328_FREERAM:
			ard_send(CMD_FREERAM,0,0,0);
			ard_recv(cmd);
			if ( cmd[0] != CMD_FREERAM ) return IOS_ERR_IOCTL;
			i = (INT32*) val;
			*i = 0;
			*i = cmd[1] | ((INT32)(cmd[2]) << 8);
		break;
			
	}
	
	return 0;
}

VOID _328_close(_H328* h)
{
	free(h);
}
