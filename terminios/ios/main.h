#ifndef __MAIN_H__
#define __MAIN_H__


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <easytype.h>
#include <ios.h>

typedef enum{CMD_NOP,CMD_EXIT,CMD_ERR,CMD_RESP,CMD_LS,CMD_EXP,CMD_EXPD,CMD_OPEN,CMD_READ,CMD_WRITE,CMD_IOCTL,CMD_CLOSE}CMDTYPE;

#define TIO_ALIAS "/home/odroid/.config/daemonios/als"
#define TIO_ALIAS_SZ 32

#define TIO_CMD_VERBOSE    0x1000
#define TIO_CMD_HELP       0x1001
#define TIO_CMD_ALIASRESET 0x1002
#define TIO_CMD_ALS        0x1003
#define TIO_CMD_EXPORTED   0x1004
#define TIO_CMD_EXPORT     0x1005
#define TIO_CMD_UNEXPORT   0x1006
#define TIO_CMD_MDELAY     0x1007
#define TIO_CMD_UDELAY     0x1008
#define TIO_CMD_NDELAY     0x1009
#define TIO_CMD_START      0x100A
#define TIO_CMD_EXIT       0x100B
#define TIO_CMD_LS         0x100C
#define TIO_CMD_OPEN       0x100D
#define TIO_CMD_CLOSE      0x100E
#define TIO_CMD_WRITE      0x100F
#define TIO_CMD_READ       0x1010
#define TIO_CMD_IOCTL      0x1011

#define DMN_COMUNICATION "/home/odroid/.config/daemonios/terminioscom.fifo"
#define DMN_DEV_MAX 1024
#define DMN_NAME_MAX 16
#define DMN_BUF0_MAX 17393
#define DMN_BUF1_MAX 512

#define TIO_SIZE_VECTOR 9999

typedef struct _MSGCMD
{
	CMDTYPE ct;
	CHAR dev[DMN_NAME_MAX];
	CHAR bf0[DMN_BUF0_MAX];
	CHAR bf1[DMN_BUF1_MAX];
	INT32 r;
	UINT32 sz;
}MSGCMD;

#define HALIAS_MAX 256

typedef struct _ALIAS
{
	CHAR name[TIO_ALIAS_SZ + 1];
	INT32 val;
	struct _ALIAS* next;
}ALIAS;

typedef struct _HALIAS
{
	ALIAS* map[HALIAS_MAX];
}HALIAS;


VOID iot_msg_print(MSGCMD* m);
VOID iot_msg_reset(MSGCMD* m);
BOOL iot_msg_send(MSGCMD* m);
BOOL iot_msg_read(MSGCMD* m);

UINT32 hls_hash(const CHAR* val, INT32 len);
VOID ali_init(HALIAS* ha);
ALIAS* ali_find(HALIAS* ha, const CHAR* name);
BOOL ali_add(HALIAS* ha, const CHAR* name, const INT32 v);
BOOL ali_remove(HALIAS* ha, const CHAR* name);
VOID ali_free(HALIAS* ha);
VOID ali_print( HALIAS* ha);

#endif
