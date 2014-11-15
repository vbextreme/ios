#include "main.h"

extern BOOL Verbose;

VOID iot_msg_reset(MSGCMD* m)
{
	m->ct = CMD_ERR;
	m->dev[0] = '\0';
	memset(m->bf0,0,DMN_BUF0_MAX);
	memset(m->bf1,0,DMN_BUF1_MAX);
	m->r = 0;
	m->sz = 0;
}

VOID iot_msg_print(MSGCMD* m)
{
	if ( m->ct == CMD_LS )
	{
		printf("%s",m->bf0);
		return;
	}
	
	if ( m->ct != CMD_RESP ) {puts("error readed command");return;}
	
	if (!Verbose) return;
	
	if ( m->sz == 0 )
		printf("%d\n",m->r);
	else
	{
		int i;
		for (i = 0; i < m->sz; i++)
			printf("%d \n",(UINT32)m->bf0[i]);
	}
}

BOOL iot_msg_send(MSGCMD* m)
{	
	INT32 f = open(DMN_COMUNICATION,O_WRONLY);
		if ( f == -1 ) return FALSE;
	if ( write(f, m, sizeof(MSGCMD) ) == -1) {close(f); return FALSE;}
	close(f);
	return TRUE;
}

BOOL iot_msg_read(MSGCMD* m)
{
	INT32 f = open(DMN_COMUNICATION,O_RDONLY);
		if ( f == -1 ) return FALSE;
	if ( read(f, m, sizeof(MSGCMD)) < sizeof(MSGCMD) ) {close(f); return FALSE;}
	close(f);
	return TRUE;
}
