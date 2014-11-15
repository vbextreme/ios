#include "main.h"

extern BOOL logmode;

int main(int argc,char** argv)
{
	if (argc == 2 && !strcmp(argv[1],"--log"))
		logmode = TRUE;
	else
		logmode = FALSE;
	
	if ( daemon(1,1) == -1 )
	{
		puts("Error on create daemonios");
		return 0;
	}
	
	openlog(DMN_LOG, LOG_PID, LOG_DAEMON);
	
	if ( !dmn_init() ) return 0;
	
	MSGCMD m;
	
	while ( dmn_msg_read(&m) )
	{	
		if ( m.ct == CMD_EXIT )
		{
			if ( logmode ) syslog(LOG_NOTICE,"Exit daemonios");
			dmn_msg_send(CMD_RESP,NULL,NULL,1,0);
			break;
		}
		
		if ( !dmn_cmd_exec(&m) )
		{ 
			syslog(LOG_ERR,"Type message error:%d",m.ct);
			dmn_msg_send(CMD_RESP,NULL,NULL,-99,0);
			continue;
		}
		
	}
	
	if ( logmode ) syslog(LOG_NOTICE,"Terminate daemonios");
	
	dmn_cortesia();
	closelog();
	
	return 0;
}
