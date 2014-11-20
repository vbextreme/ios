#include "main.h"

extern BOOL logmode;
BOOL syncmode;

int main(int argc,char** argv)
{
	logmode = FALSE;
	syncmode = FALSE;
	
	++argv;
	while ( *argv )
	{
		if ( !strcmp(*argv,"--log") || !strcmp(*argv,"-l"))
			logmode = TRUE;
		else if ( !strcmp(*argv,"--sync") || !strcmp(*argv,"-s"))
			syncmode = TRUE;
	}
	
	if ( daemon(1,1) == -1 )
	{
		puts("Error on create daemonios");
		return 0;
	}
	
	openlog(DMN_LOG, LOG_PID, LOG_DAEMON);
	
	if ( !dmn_init() ) return 0;
	
	ard_init(syncmode);
	
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
	ard_close();
	dmn_cortesia();
	closelog();
	
	return 0;
}
