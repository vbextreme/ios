#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <easytype.h>
#include <ios.h>


typedef enum{CMD_NOP,CMD_EXIT,CMD_ERR,CMD_RESP,CMD_LS,CMD_EXP,CMD_EXPD,CMD_OPEN,CMD_READ,CMD_WRITE,CMD_IOCTL,CMD_CLOSE}CMDTYPE;

#define DMN_LOG "daemonios"
#define DMN_COMUNICATION "/home/odroid/.config/daemonios/terminioscom.fifo"
#define DMN_DEV_MAX 1024
#define DMN_NAME_MAX 16
#define DMN_BUF0_MAX 17393
#define DMN_BUF1_MAX 512

typedef struct _MSGCMD
{
	CMDTYPE ct;
	CHAR dev[DMN_NAME_MAX];
	CHAR bf0[DMN_BUF0_MAX];
	CHAR bf1[DMN_BUF1_MAX];
	INT32 r;
	UINT32 sz;
}MSGCMD;

BOOL dmn_init();
VOID dmn_cortesia();
BOOL dmn_msg_send(CMDTYPE ct, CHAR* bf0, CHAR* bf1, INT32 r, UINT32 sz);
BOOL dmn_msg_read(MSGCMD* m);
BOOL dmn_cmd_exec(MSGCMD* m);

#endif
