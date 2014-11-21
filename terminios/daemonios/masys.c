#include "main.h"

static HIOS _h[DMN_DEV_MAX];
static CHAR _hn[DMN_DEV_MAX][DMN_NAME_MAX];

BOOL logmode;

VOID _msg_reset(MSGCMD* m)
{
	m->ct = CMD_ERR;
	m->dev[0] = '\0';
	m->bf0[0] = '\0';
	m->bf1[0] = '\0';
	m->r = 0;
	m->sz = 0;
}

BOOL dmn_init()
{
	if ( logmode ) syslog(LOG_NOTICE,"Start daemonios");
	
	unlink(DMN_COMUNICATION);
	
	if ( mkfifo(DMN_COMUNICATION,0666) == -1 ) 
	{
		syslog(LOG_ERR,"[Error]error create fifo");
		return FALSE;
	}
	
	register UINT32 i;
	for (i = 0; i < DMN_DEV_MAX; i++)
	{
		_h[i] = NULL;
		_hn[i][0] = '\0';
	}
	
	return TRUE;
}

VOID dmn_cortesia()
{
	unlink(DMN_COMUNICATION);
	
	register UINT32 i;
	for (i = 0; i < DMN_DEV_MAX; i++)
		if (_h[i]) ios_close(_h[i]);
}

INT32 dmn_getih()
{
	register UINT32 i;
	for (i = 0; i < DMN_DEV_MAX; i++)
		if ( !_h[i] ) return i;
	return -1;
}

INT32 dmn_getifn(CHAR* n)
{
	register UINT32 i;
	for (i = 0; i < DMN_DEV_MAX; i++)
		if (_h[i] && !strcmp(_hn[i],n) ) return i;
	return -1;
}

VOID dmn_ls(CHAR* d)
{
	register UINT32 i;
	*d = '\0';
	
	for (i = 0; i < DMN_DEV_MAX; i++)
	{
		if ( _h[i] ) 
		{
			strcpy(d,_hn[i]);
			d += strlen(_hn[i]);
			*d++ = '\n';
			*d = '\0';
		}
	}
}

BOOL dmn_msg_send(CMDTYPE ct, CHAR* bf0, CHAR* bf1, INT32 r, UINT32 sz)
{
	if ( logmode ) syslog(LOG_NOTICE,"[Notice]Message Send:%d",ct);
	
	MSGCMD m;
	
	m.ct = ct;
	m.dev[0] = '\0';
	
	if ( bf0 )
		strcpy(m.bf0,bf0);
	else
		m.bf0[0] = '\0';
	
	if ( bf1 )
		strcpy(m.bf1,bf1);
	else
		m.bf1[0] = '\0';
	
	m.r = r;
	m.sz = sz;
	
	INT32 f = open(DMN_COMUNICATION,O_WRONLY | O_NONBLOCK);
		if ( f == -1 ) {syslog(LOG_ERR,"[Error]Comunication writing error open");return FALSE;}
	if ( write(f, &m, sizeof(MSGCMD) ) < 1) {syslog(LOG_ERR,"[Error]Comunication error write");close(f); return FALSE;}
	close(f);
	return TRUE;
}

BOOL dmn_msg_read(MSGCMD* m)
{
	if ( logmode ) syslog(LOG_NOTICE,"[Notice]Message recv");
	
	INT32 f = open(DMN_COMUNICATION,O_RDONLY);
		if ( f == -1 ) {syslog(LOG_ERR,"[Error]Comunication reading error open");return FALSE;}
	_msg_reset(m);
	
	fd_set set;
	struct timeval timeout;
	int rv;
	FD_ZERO(&set); /* clear the set */
	FD_SET(f, &set); /* add our file descriptor to the set */

	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;
	
	syslog(LOG_ERR,"[Info]Try read");

	rv = select(FD_SETSIZE, &set, NULL, NULL,&timeout);
	if ( rv == -1 )
    {
		syslog(LOG_ERR,"[Error]Comunication select");
		close(f);
		return FALSE;
	}
	else if ( rv == 0 )
	{
		syslog(LOG_ERR,"[Error]Comunication timeout");
		close(f);
		return FALSE;
	}

	if ( read(f, m, sizeof(MSGCMD)) < sizeof(MSGCMD) ) {syslog(LOG_ERR,"[Error]Comunication error reading");close(f); return FALSE;}
	close(f);
	
	syslog(LOG_ERR,"[Info]ok");
	return TRUE;
}

BOOL dmn_cmd_exec(MSGCMD* m)
{
	if ( logmode ) syslog(LOG_NOTICE,"[Notice]Message exec:%d",m->ct);
	
	INT32 ih,mode;
	BYTE rbuf[DMN_BUF0_MAX];
	
	switch (m->ct)
	{
		case CMD_NOP: 
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command NOP");
			mode = 1;
		break;
		
		case CMD_ERR: 
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Error");
		return FALSE;
		
		case CMD_EXIT: 
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Exit");
		return FALSE;
		
		case CMD_EXP:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Export");
			mode = ( ios_export(m->r,m->sz) ) ? 0 : 1; 
			if ( logmode ) syslog(LOG_INFO,"[Info]mode:%d",mode);
			dmn_msg_send(CMD_RESP,NULL,NULL,mode,0);
		return TRUE;
		
		case CMD_EXPD:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Exported");
			mode = ios_exported(m->r);
			dmn_msg_send(CMD_RESP,NULL,NULL,mode,0);
		return TRUE;
		
		case CMD_OPEN:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Open");
			if ( dmn_getifn(m->dev) != -1 ) 
			{ 
				if ( logmode ) syslog(LOG_INFO,"[Info]Name:\"%s\" alredy exist",m->dev);
				mode = -1; 
				break;
			}
			ih = dmn_getih();
			if ( ih < 0 ) 
			{ 
				if ( logmode ) syslog(LOG_WARNING,"[Warning]Max Number device");
				mode = -2; 
				break;
			}
			
			if ( logmode ) syslog(LOG_INFO,"[Info]name:\"%s\" ih:%d path:\"%s\" mode:\"%s\"",m->dev,ih,m->bf0,m->bf1); 
				
			if ( !(_h[ih] = ios_open(m->bf0,m->bf1)) ) 
			{ 
				syslog(LOG_ERR,"[Error]IOS can't open device");
				mode = -3; 
				break;
			}
			strcpy(_hn[ih],m->dev);
			mode = 1;
		break;
		
		case CMD_READ:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Read");
			if ( (ih = dmn_getifn(m->dev)) == -1 ) 
			{
				if ( logmode ) syslog(LOG_INFO,"[Info]No device:%s",m->dev); 
				mode = -1; 
				break;
			}
			if ( !ios_read(_h[ih],rbuf,m->sz,m->r) ) 
			{
				syslog(LOG_ERR,"[Error]IOS can't read device"); 
				mode = -4; 
				break;
			}
			dmn_msg_send(CMD_RESP,(CHAR*)rbuf,NULL,m->r,m->sz);
		return TRUE;
		
		case CMD_WRITE:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Write");
			if ( (ih = dmn_getifn(m->dev)) == -1 ) 
			{ 
				if ( logmode ) syslog(LOG_INFO,"[Info]No device:%s",m->dev); 
				mode = -1; 
				break;
			}
			if ( !ios_write(_h[ih],m->bf0,m->sz,m->r) ) 
			{
				syslog(LOG_ERR,"[Error]IOS can't write device:ih:%d,v:%d,sz:%d,n:%d",ih,*((INT32*)(m->bf0)),m->sz,m->r); 
				mode = -5; 
				break;
			}
			mode = 1;
		break;
		
		case CMD_IOCTL:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Ioctl");
			if ( (ih = dmn_getifn(m->dev)) == -1 )
			{ 
				if ( logmode ) syslog(LOG_INFO,"[Info]No device:%s",m->dev); 
				mode = -1; 
				break;
			}
			if ( (mode = ios_ioctl(_h[ih],m->r,m->bf0)) ) 
			{ 
				syslog(LOG_ERR,"[Error]IOS error ioctl:%d to req:%d value:%d",mode,m->r,*((INT32*)(m->bf0))); 
				mode = -6; 
				break;
			}
			mode = 1;
		break;
		
		case CMD_CLOSE:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command Close");
			
			if ( (ih = dmn_getifn(m->dev)) == -1 )
			{ 
				if ( logmode ) syslog(LOG_INFO,"[Info]No device:%s",m->dev); 
				mode = -1; 
				break;
			}
			ios_close(_h[ih]);
			_h[ih] = NULL;
			_hn[ih][0]= '\0';
			mode = 1;
		break;
		
		case CMD_LS:
			if ( logmode ) syslog(LOG_NOTICE,"[Notice]Command ls");
			dmn_ls((CHAR*)rbuf);
			dmn_msg_send(CMD_LS,(CHAR*)rbuf,NULL,0,strlen((CHAR*)rbuf));
		return TRUE;
		
		default: 
			syslog(LOG_ERR,"[Error]Command undefinited"); 
		return FALSE;
	}
	
	dmn_msg_send(CMD_RESP,NULL,NULL,mode,0);
	return TRUE;
}
