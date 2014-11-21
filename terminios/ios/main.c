#include "main.h"
#include <errno.h>

#define PID_CHILD 0
#define _verbose(MSG) if ( Verbose ) puts(MSG)
#define _TESTP(N) if ( !*(argv+(N)) ) {puts("Error arg");return -3;}

HALIAS ha;
BOOL Verbose = FALSE;

void help()
{
	puts("-----------------------");
	puts("- INPUT OUTPUT SHIELD -");
	puts("-------------------------------------------------------");
	puts("- VALUE  ---                 DESCRIPT                 -");
	puts("-------------------------------------------------------");
	puts("- pin    --- accept real value 289->304 or            -");
	puts("-            alias value p0->p7 p10->p17              -");
	puts("-            d0->d13 a0->a15                          -");
	puts("- device --- max 15 char                              -");
	puts("- path   --- libname/devtype                          -");
	puts("- mode   --- r = read w = write or any for spec.      -");
	puts("- value  --- value is send by byte                    -");
	puts("- size   --- -d = 2 byte, -D = 4 byte -b = 1 byte     -");
	puts("- req    --- int value                                -");
	puts("-------------------------------------------------------");
	puts("- COMMAND  ---        DESCRIPT        -    RETURN     -");
	puts("-------------------------------------------------------");
	puts("- start    --- run daemonios          - started / err -");
	puts("- exit     --- kill daemonios         - 1/0           -");
	puts("- ls       --- list of device         - all device    -");
	puts("- als      --- list of alias          - all alias     -");
	puts("-------------------------------------------------------");
	puts("- COMMAND  - ARG      -        DESCRIPT      - RETURN -");
	puts("-------------------------------------------------------");
	puts("- export   - pin      - export pin           - 1/0    -");
	puts("- unexport - pin      - export pin           - 1/0    -");
	puts("- exported - pin      - check pin is export  - 1/0    -");
	puts("------------------------------------------------------------------------");
	puts("-                      DEFAULT COMMAND MODE                            -");
	puts("------------------------------------------------------------------------");
	puts("- ios [device name] [type cmd] [argument]                              -");
	puts("------------------------------------------------------------------------");
	puts("- TYPECMD -          ARG          -          DESCRIPT         - RETURN -");
	puts("------------------------------------------------------------------------");
	puts("-   -o    -        w/W r/R        - Create and open new device-   1/0  -");
	puts("-   -r    - {-sz(default 1)}      - read nbyte from device    -  value -");
	puts("-   -w    - {-sz(default 1)}value - write nbyte to device,    -        -");
	puts("-         -                       - multiple value sep space  -   1/0  -");
	puts("-   -i    - req {-sz(default 4)}v - set ioctl                 -   1/0  -");
	puts("-   -c    -                       - close device              -   1/0  -");
	puts("------------------------------------------------------------------------");
	puts("");	
}

void aliasreset(CHAR* patha)
{
	remove(patha);
	
	FILE* f = fopen(patha,"w");
		if (!f ) return;
	
	fprintf(f,"--verbose = 0x%X\n", TIO_CMD_VERBOSE);
	fprintf(f,"-v = 0x%X\n", TIO_CMD_VERBOSE);
	fprintf(f,"--help = 0x%X\n", TIO_CMD_HELP);
	fprintf(f,"-h = 0x%X\n", TIO_CMD_HELP);
	fprintf(f,"aliasreset = 0x%X\n", TIO_CMD_ALIASRESET);
	fprintf(f,"als = 0x%X\n", TIO_CMD_ALS);
	fprintf(f,"--help = 0x%X\n", TIO_CMD_HELP);
	fprintf(f,"exported = 0x%X\n", TIO_CMD_EXPORTED);
	fprintf(f,"export = 0x%X\n", TIO_CMD_EXPORT);
	fprintf(f,"unexport = 0x%X\n", TIO_CMD_UNEXPORT);
	fprintf(f,"mdelay = 0x%X\n", TIO_CMD_MDELAY);
	fprintf(f,"udelay = 0x%X\n", TIO_CMD_UDELAY);
	fprintf(f,"ndelay = 0x%X\n", TIO_CMD_NDELAY);
	fprintf(f,"start = 0x%X\n", TIO_CMD_START);
	fprintf(f,"exit = 0x%X\n", TIO_CMD_EXIT);
	fprintf(f,"ls = 0x%X\n", TIO_CMD_LS);
	fprintf(f,"-o = 0x%X\n", TIO_CMD_OPEN);
	fprintf(f,"--open = 0x%X\n", TIO_CMD_OPEN);
	fprintf(f,"-c = 0x%X\n", TIO_CMD_CLOSE);
	fprintf(f,"--close = 0x%X\n", TIO_CMD_CLOSE);
	fprintf(f,"-r = 0x%X\n", TIO_CMD_READ);
	fprintf(f,"--read = 0x%X\n", TIO_CMD_READ);
	fprintf(f,"-w = 0x%X\n", TIO_CMD_WRITE);
	fprintf(f,"--write = 0x%X\n", TIO_CMD_WRITE);
	fprintf(f,"-i = 0x%X\n", TIO_CMD_IOCTL);
	fprintf(f,"--ioctl = 0x%X\n", TIO_CMD_IOCTL);
	
	INT32 i;
	
	for ( i = 0; i < 8; ++i)
		fprintf(f,"p%d = 0x%X\n", i, IOS_P00 + i);
		
	for ( i = 10; i < 18; ++i)
		fprintf(f,"p%d = 0x%X\n", i, IOS_P10 + i);
	
	for ( i = 0; i < 13; ++i)
		fprintf(f,"d%d = 0x%X\n", i, IOS_D00 + i);
	
	for ( i = 0; i < 6; ++i)
		fprintf(f,"a%d = 0x%X\n", i, IOS_A00 + i);
	
	fprintf(f,"pinset = 0x%X\n", IOS_IOCTL_PIN_SET);
	fprintf(f,"high = 0x%X\n", 1);
	fprintf(f,"low = 0x%X\n", 0);
	fprintf(f,"on = 0x%X\n", 1);
	fprintf(f,"off = 0x%X\n", 0);
	
	for ( i = 0; i < 8; ++i)
		fprintf(f,"portset%d = 0x%X\n" ,i, IOS_IOCTL_PORT_SET_0 + i);
	
	//fprintf(f,"portseta = 0x%X\n", IOS_IOCTL_PORT_SET_A);
	
	fprintf(f,"u3lmmc = 0x%X\n", IOS_IOCTL_U3L_MMC);
	fprintf(f,"u3lgpio = 0x%X\n", IOS_IOCTL_U3L_GPIO);
	fprintf(f,"u3loneshot = 0x%X\n", IOS_IOCTL_U3L_ONESHOT);
	fprintf(f,"u3lsd = 0x%X\n", IOS_IOCTL_U3L_SD);
	fprintf(f,"u3ltimer = 0x%X\n", IOS_IOCTL_U3L_TIMER);
	fprintf(f,"u3ltimeroff = 0x%X\n", IOS_IOCTL_U3L_TIMER_OFF);
	fprintf(f,"u3ltimeron = 0x%X\n", IOS_IOCTL_U3L_TIMER_ON);
	
	fprintf(f,"pulseset = 0x%X\n", IOS_IOCTL_PULSE_SET);
	fprintf(f,"pulsemode = 0x%X\n", IOS_IOCTL_PULSE_MODE);
	fprintf(f,"pulseus = 0x%X\n", IOS_IOCTL_PULSE_US);

	fprintf(f,"pwmwait = 0x%X\n", IOS_IOCTL_PWM_WAIT);
	fprintf(f,"pwmset = 0x%X\n", IOS_IOCTL_PWM_SET);
	fprintf(f,"pwmduty = 0x%X\n", IOS_IOCTL_PWM_SET_DUTY);
	fprintf(f,"pwmfq = 0x%X\n", IOS_IOCTL_PWM_SET_FQ);
	fprintf(f,"pwmtout = 0x%X\n", IOS_IOCTL_PWM_SET_TOUT);
	fprintf(f,"pwmendmode = 0x%X\n", IOS_IOCTL_PWM_END_MODE);
	fprintf(f,"pwmreset = 0x%X\n", IOS_IOCTL_PWM_READ_RESET);
	fprintf(f,"pwmelapse = 0x%X\n", IOS_IOCTL_PWM_ELAPSE);
	
	fprintf(f,"-b = 0x%X\n", 1);
	fprintf(f,"-d = 0x%X\n", 2);
	fprintf(f,"-D = 0x%X\n", 4);
	fprintf(f,"-s = 0x%X\n", 0);
	fprintf(f,"-v = 0x%X\n", TIO_SIZE_VECTOR + 1);
	fprintf(f,"-vd = 0x%X\n", TIO_SIZE_VECTOR + 4);

	fclose(f);
}

INT32 aliasinit(CHAR* patha)
{
	ali_init(&ha);
	
	FILE* f = fopen(patha,"r");
	if ( !f ) 
	{
		aliasreset(patha);
		if ( !(f = fopen(patha,"r")) ) return 0;
	}
	
	CHAR line[256];
	CHAR newa[TIO_ALIAS_SZ + 1];
	CHAR val[TIO_ALIAS_SZ + 1];
	CHAR* p;
	INT32 sz;
		
	while ( fgets(line,254,f) )
	{
		p = line;
		while( *p && (*p == ' ' || *p == '\t') ) ++p;
		for ( sz = 0; *p && *p != ' ' && *p != '\t' && *p != '=' && sz < TIO_ALIAS_SZ; newa[sz++] = *p++ );
		newa[sz] = '\0';
		if ( !*p ) break;
		
		while( *p && (*p == ' ' || *p == '\t') ) ++p;
		if ( *p++ != '=' ) break;
		
		while( *p && (*p == ' ' || *p == '\t') ) ++p;
		for ( sz = 0; *p && *p != ' ' && *p != '\t' && *p != '\n' && sz < TIO_ALIAS_SZ; val[sz++] = *p++ );
		val[sz] = '\0';
		//printf("debug:%s = %s = %d = 0x%X\n",newa , val, (INT32)strtol(val,NULL,0),(INT32)strtol(val,NULL,0));
		ali_add( &ha, newa , (INT32)strtol(val,NULL,0));
	}
	
	fclose(f);
	return 1;
}

INT32 aliasins(CHAR* patha, CHAR* pathins)
{
	
	FILE* fd = fopen(patha,"w");
	if ( !fd ) 
	{
		aliasreset(patha);
		if ( !(fd = fopen(patha,"w")) ) return 0;
	}
	
	fseek(fd,0,SEEK_END);
	
	FILE* fs = fopen(pathins,"r");
		if ( !fs ) { fclose(fd); return 0;}
	
	CHAR line[256];
	
	while ( fgets(line,254,fs) )
	{
		if (line[strlen(line)-1] != '\n')
			fprintf(fd,"%s\n",line);
		else
			fprintf(fd,"%s",line);
	}
	
	fclose(fd);
	fclose(fs);
	return 1;
}

INT32 _comunication(MSGCMD* m)
{
	if ( !iot_msg_send(m) ) {puts("error to send request");return -7;}
	iot_msg_reset(m);
	if ( !iot_msg_read(m) ) {puts("error to read request");return -7;}
	return 0;
}

BOOL _setpin(INT32* p, CHAR* arg)
{
	ALIAS* f = ali_find( &ha, arg);
	if ( !f )
	{
		*p = (INT32)strtol(arg,NULL,0);
		return ( *p <= 0 ) ? FALSE : TRUE;
	}
	
	*p = f->val;
	//printf("ON ALIAS:%d\n",f->val);
	return TRUE;
}

BOOL _setsz(UINT32* sz, CHAR* arg)
{
	if ( *arg != '-' ) return FALSE; 
	ALIAS* f = ali_find( &ha, arg);
		if ( !f ) return FALSE;
	*sz = f->val;
	return TRUE;
}

INT32 _valarg(CHAR* arg)
{
	ALIAS* f = ali_find( &ha, arg);
	return ( !f ) ? (INT32)strtol(arg,NULL,0) : f->val;
}

INT32 startexit(INT32 mode,CHAR* argd)
{
	if ( mode )
	{
		INT32 p;
		 
		if ( ( p = fork() ) == -1 ) {puts("error to fork process");	return 0;}
		
		if ( 0 == p )
		{
			if (argd)
				execl("/usr/bin/daemonios","daemonios",argd,NULL);
			else
				execl("/usr/bin/daemonios","daemonios",NULL);
			printf("error and exit:%d\n",errno);
			return -1;
		}
		
		waitpid(p,NULL,0);
		
		return 1;
	}
	
	MSGCMD m;
		iot_msg_reset(&m);
	m.ct = CMD_EXIT;
	if ( _comunication(&m) ) return 0;
	return 1;
}

int main(int argc,char** argv)
{	
	if ( argc > 2 && !strcmp(*(argv+1),"--filealias") )
	{
		argc -= 2;
		if ( !aliasinit(*(argv+2)) ) {puts("Error to initialize user alias"); return -1;}
		argv += 2;
	}
	else
	{
		if ( !aliasinit(TIO_ALIAS) ) {puts("Error to initialize alias"); return -1;}
	}
	
	MSGCMD m;
	iot_msg_reset(&m);

	--argc;
	++argv;
	if (argc < 1) { puts("-h for help"); return -1;}

	INT32 state = 0;
	INT32 iarg = 0;
	INT32 exmode = IOS_EXPORT;

	while ( *argv )
	{
		ALIAS* f = ali_find( &ha, *argv);
		if ( !f )
		{
			if ( !state ) 
			{
				strcpy(m.dev,*argv);
				state = 1;
				++argv;
				continue;
			}
			puts("command not found"); 
			return -2;
		}		
		
		switch ( f->val )
		{
			case TIO_CMD_ALIASRESET:
				aliasreset(TIO_ALIAS);
				_verbose("Ok");
			return 0;
			
			case TIO_CMD_ALS:
				ali_print(&ha);
			return 0;
			
			case TIO_CMD_ALINS:
				_TESTP(1)
				if ( !aliasins(TIO_ALIAS,*(argv+1)) )
					printf("error to read new alias");
				else
					_verbose("add new alias");
			return 0;
			
			case TIO_CMD_MDELAY:
				_TESTP(1)
				ios_msleep( atoi(*(argv+1)) );
				_verbose("end sleep");
			return 0;
			
			case TIO_CMD_UDELAY:
				_TESTP(1)
				ios_usleep( atoi(*(argv+1)) );
				_verbose("end sleep");
			return 0;
			
			case TIO_CMD_NDELAY:
				_TESTP(1)
				ios_nsleep( atoi(*(argv+1)) );
				_verbose("end sleep");
			return 0;
			
			case TIO_CMD_VERBOSE:
				Verbose = TRUE;
			break;
			
			case TIO_CMD_HELP:
				help();
			return 0;
			
			case TIO_CMD_START:
				if ( startexit(1,*(argv+1)) ) 
				{
					_verbose("start daemonios");
					return 0;
				}
				puts("error to start daemonios");
			return -5;
			
			case TIO_CMD_EXIT:
				if ( startexit(0,NULL) ) 
				{
					_verbose("killed daemonios");
					return 0;
				}
				puts("error to kill daemonios");
			return -5;
			
			case TIO_CMD_UNEXPORT:
				exmode = IOS_UNEXPORT;
			case TIO_CMD_EXPORT:
				_TESTP(1)
				++argv;
				if ( !strcmp(*argv,"all") ) 
				{
					if ( (iarg = ios_exportall(exmode)) ) { printf("error:%d to un/export all pin\n",iarg); return iarg;}
					_verbose("all ok");
					return 0;
				}
				if ( !_setpin(&iarg,*argv) ) { puts("error pin"); return -6;}
				if ( !ios_export(iarg, exmode) ) {	puts("error to un/export pin"); return -6;	}
				_verbose("ok");
			return 0;
			
			case TIO_CMD_EXPORTED:
				_TESTP(1)
				if ( !_setpin(&iarg,*(argv+1)) ) { puts("error pin"); return -6;}
				printf("%d\n",ios_exported(iarg));
			return 0;
			
			case TIO_CMD_LS:
				m.ct = CMD_LS;
				if ( _comunication(&m) ) {return -7;}
				iot_msg_print(&m);
			return 0;
			
			case TIO_CMD_OPEN:
				if ( state != 1 ) {puts("undefinited device"); return -7;}
				m.ct = CMD_OPEN;
				_TESTP(1);
				strcpy(m.bf0,*(argv+1));
				_TESTP(2);
				strcpy(m.bf1,*(argv+2));
				if ( _comunication(&m) ) {return -7;}
				iot_msg_print(&m);
			return 0;
			
			case TIO_CMD_CLOSE:
				if ( state != 1 ) {puts("undefinited device"); return -7;}
				m.ct = CMD_CLOSE;
				if ( _comunication(&m) ) {return -7;}
				iot_msg_print(&m);
			return 1;
			
			case TIO_CMD_READ:
				if ( state != 1 ) {puts("undefinited device"); return -7;}
				m.ct = CMD_READ;
				m.r = 1;
				_TESTP(1);
				++argv;
				if ( !_setsz(&m.sz,*argv) ) m.sz = 2;
				if ( _comunication(&m) ) {return -7;}
				iarg = Verbose;
					Verbose = TRUE;
					iot_msg_print(&m);
				Verbose = iarg;
			return 0;
			
			case TIO_CMD_WRITE:
				if ( state != 1 ) {puts("undefinited device"); return -7;}
				m.ct = CMD_WRITE;
				m.r = 1;
				_TESTP(1);
				++argv;
				if ( !_setsz(&m.sz,*argv) ) 
					{m.sz = 1;}
				else
				{
					_TESTP(1)
					++argv;
				}
				
				if (m.sz == 0)
				{
					m.sz = strlen(*argv);
					strcpy(m.bf0,*argv);
				}
				else if ( m.sz > TIO_SIZE_VECTOR )
				{
					INT32 esz = m.sz - TIO_SIZE_VECTOR;
					m.sz = _valarg(*argv);	
					INT32 i;
					CHAR* m0 = m.bf0;
					for ( i = 0; i < m.sz; ++i, m0 += esz)
					{
						_TESTP(1)
						++argv;
						iarg = _valarg(*argv);	
						memcpy(m0,&iarg,esz);
					}	
				}
				else	
				{
					iarg = _valarg(*argv);	
					memcpy(m.bf0,&iarg,m.sz);
				}
				//printf("debuw:%d %d %d\n",m.r,m.sz,iarg);
				
				if ( _comunication(&m) ) {return -7;}
				iot_msg_print(&m);
			return 0;
			
			case TIO_CMD_IOCTL:
				if ( state != 1 ) {puts("undefinited device"); return -7;}
				m.ct = CMD_IOCTL;
				
				_TESTP(1)
				++argv;
				m.r = _valarg(*argv);
				
				_TESTP(1)
				++argv;
				if ( !_setsz(&m.sz,*argv) ) 
					{m.sz = 4;}
				else
					{_TESTP(1) ++argv;}
				
				if (m.sz == 0)
				{
					m.sz = strlen(*argv);
					strcpy(m.bf0,*argv);
				}
				else if ( m.sz > TIO_SIZE_VECTOR )
				{
					INT32 esz = m.sz - TIO_SIZE_VECTOR;
					m.sz = _valarg(*argv);	
					INT32 i;
					CHAR* m0 = m.bf0;
					for ( i = 0; i < m.sz; ++i, m0 += esz)
					{
						_TESTP(1)
						++argv;
						iarg = _valarg(*argv);	
						memcpy(m0,&iarg,esz);
					}	
				}
				else	
				{
					iarg = _valarg(*argv);	
					memcpy(m.bf0,&iarg,m.sz);
				}
				
				//printf("debui:%d %d %d\n",m.r,m.sz,iarg);
				
				if ( _comunication(&m) ) {return -7;}
				iot_msg_print(&m);
			return 0;
			
			default:
				puts("undefinited command"); 
			return -2; 
				
		}//switch command
		
		++argv;
	}//loop arg
	
	puts("no arg undefinited command"); 
	return -2;
}
