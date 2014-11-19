#ifndef __PRVIOS_H__
#define __PRVIOS_H__

// pin:p0
// port/p0.p2.p3.p4.p5.p6.p7
// pwm/p0.duty.fq.timeout

#include "ios.h"
#include <easytype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <easythread.h>
#include <easyserial.h>

#define PATH_DL        "/usr/lib/"
#define FILE_ULED      "/sys/class/leds/led1/"
#define FILE_PIN       "/sys/class/gpio/gpio"
#define FILE_DIRECTION "/direction"
#define FILE_VALUE     "/value"
#define FILE_EXPORT    "/sys/class/gpio/export"
#define FILE_UNEXPORT  "/sys/class/gpio/unexport"
#define FILE_COM       "/dev/ttySAC0"
#define COM_SPEED      9600
#define COM_WAIT      30000
#define COM_CMD           4
#define MAX_INPATH     45

#define IOS_WAITTHR 3000

#define IOS_LCDA_NORW 255
#define IOS_LCDA_CLEARDISPLAY 0x01
#define IOS_LCDA_RETURNHOME 0x02
#define IOS_LCDA_ENTRYMODESET 0x04
#define IOS_LCDA_DISPLAYCONTROL 0x08
#define IOS_LCDA_CURSORSHIFT 0x10
#define IOS_LCDA_FUNCTIONSET 0x20
#define IOS_LCDA_SETCGRAMADDR 0x40
#define IOS_LCDA_SETDDRAMADDR 0x80
#define IOS_LCDA_ENTRYRIGHT 0x00
#define IOS_LCDA_ENTRYLEFT 0x02
#define IOS_LCDA_ENTRYSHIFTINCREMENT 0x01
#define IOS_LCDA_ENTRYSHIFTDECREMENT 0x00
#define IOS_LCDA_DISPLAYON 0x04
#define IOS_LCDA_DISPLAYOFF 0x00
#define IOS_LCDA_CURSORON 0x02
#define IOS_LCDA_CURSOROFF 0x00
#define IOS_LCDA_BLINKON 0x01
#define IOS_LCDA_BLINKOFF 0x00
#define IOS_LCDA_DISPLAYMOVE 0x08
#define IOS_LCDA_CURSORMOVE 0x00
#define IOS_LCDA_MOVERIGHT 0x04
#define IOS_LCDA_MOVELEFT 0x00
#define IOS_LCDA_8BITMODE 0x10
#define IOS_LCDA_4BITMODE 0x00
#define IOS_LCDA_2LINE 0x08
#define IOS_LCDA_1LINE 0x00
#define IOS_LCDA_5x10DOTS 0x04
#define IOS_LCDA_5x8DOTS 0x00
#define IOS_LCDA_MODE_COMMAND IOS_PIN_LOW
#define IOS_LCDA_MODE_WRITE   IOS_PIN_HIGH


///328 cmd
#define CMD_ECHO         0x00
#define CMD_ERR          0x01
#define CMD_SLEEP        0x02
#define CMD_OPZ          0x03
#define CMD_MAXTO        0x04
#define CMD_PIN_DIR      0x05
#define CMD_PIN_WRITE    0x06
#define CMD_PIN_READ     0x07
#define _CMD_PWM_OUT     0x08
#define CMD_PWM_FQ_SET   0x09
#define CMD_PWM_DT_SET   0x0A
#define CMD_PWM_TOUT_SET 0x0B
#define CMD_PWM_ENABLE   0x0C
#define CMD_PWM_DISABLE  0x0D
#define CMD_PRE_SET      0x0E
#define CMD_PWM_AUTO     0x0F
#define CMD_MEM_STA      0x10
#define CMD_MEM_WRITE    0x11
#define CMD_MEM_READ     0x12
#define CMD_NOP          0x13
#define CMD_OPPC         0x14
#define CMD_RR           0x15
#define CMD_RI           0x16
#define CMD_RM           0x17
#define CMD_MR           0x18
#define CMD_ADD          0x19
#define CMD_DIF          0x1A
#define CMD_MUL          0x1B
#define CMD_DIV          0x1C
#define CMD_SHL          0x1D
#define CMD_SHR          0x1E
#define CMD_OR           0x1F
#define CMD_AND          0x20
#define CMD_XOR          0x21
#define CMD_NOT          0x22
#define CMD_JMP          0x23
#define CMD_JMI          0x24
#define CMD_JMA          0x25
#define CMD_JEQ          0x26
#define CMD_JAN          0x27
#define CMD_JOR          0x28
#define CMD_JNO          0x29
#define CMD_PUSH         0x2A
#define CMD_POP          0x2B
#define CMD_PUSH_REG     0x2C
#define CMD_POP_REG      0x2D
#define CMD_CALL         0x2E
#define CMD_RET          0x2F
#define CMD_OVDT         0x30
#define CMD_PULSED_OUT   0x31
#define CMD_PULSEU_OUT   0x32
#define CMD_PULSEF_IN    0x33
#define CMD_PULSEC_IN    0x34
#define CMD_MDELAY       0x35
#define CMD_UDELAY       0x36
#define CMD_FREERAM      0x37

typedef enum {HM_LDL,HM_U3L,HM_PIN,HM_PORT,HM_PULSE,HM_INT,HM_PWM,HM_328,HM_LCDA} HMODE;

typedef VOID* (*_IOSOPEN)(CHAR*);
typedef INT32 (*_IOSWRITE)(VOID*,const VOID*,UINT32,UINT32);
typedef INT32 (*_IOSREAD)(VOID*,VOID*,UINT32,UINT32);
typedef VOID (*_IOSCLOSE)(VOID*);
typedef INT32 (*_IOSIOCTL)(VOID* h, INT32 req, VOID* s);

typedef struct __HIOS
{
	VOID* p;
	HMODE m;
	_IOSWRITE w;
	_IOSREAD r;
	_IOSCLOSE c;
	_IOSIOCTL i;
	VOID* dl;
}_HIOS;

typedef struct __HU3L
{
	INT32  f;
}_HU3L;

typedef struct __HIO
{
	UINT32 p;
	INT32  d;
	INT32  f;
	BOOL   a;
}_HIO;

typedef struct __HPORT
{
	_HIO* p[8];
	INT32  d;
}_HPORT;

typedef struct __HPULSE
{
	_HIO*   p;
	INT32   d;
	BYTE    s;
	INT32  us;
	UINT32 ck;
	INT32  mr;
	INT32   a;
}_HPULSE;

typedef struct __HPWM
{
	_HIO* hp;
	INT32 d;
	UINT32 p;
	
	THR t;
	FLOAT64 per;
	FLOAT64 don;
	BOOL ent;
	FLOAT64 remt;
	
	BYTE    em;
	UINT32  fq;
	UINT32 duty;
	UINT32 tout;
	FLOAT64 elapse;
}_HPWM;

typedef struct __HINT
{
	THR t;
	_HIO* p;
	INT32 intset;
	INT32 mode;
	CHAR ph[IOS_PATH_MAX];
	IOSINT cbk;
	VOID* param;
	UINT32 bou;
}_HINT;

typedef struct __H328
{
	INT32 stwr;
	INT32 ppwa;
}_H328;

typedef struct __HLCDA
{
	_HIO* hrs;
	_HIO* hrw;
	_HIO* he;
	_HIO* hd[8];
	BOOL nibble;
	BYTE nline;
	BYTE cline;
	BOOL dotsz;
	BYTE dspcnt;
	BYTE dspmode;
	BYTE dspfnc;
}_HLCDA;

#ifdef __cplusplus
extern "C" {
#endif

UINT32 ard_send(BYTE cmd, BYTE p0, BYTE p1, BYTE p2);
UINT32 ard_recs(BYTE* cmd, BYTE* p0, BYTE* p1, BYTE* p2);
UINT32 ard_recv(BYTE* cppp);

_HU3L* _u3l_open(CHAR* mode);
INT32 _u3l_ioctl(_HU3L* h, INT32 req, VOID* val);
INT32 _u3l_write(_HU3L* h,const VOID* v, UINT32 sz, UINT32 n);
VOID _u3l_close(_HU3L* h);

_HIO* _pin_open(CHAR* mode);
INT32 _pin_ioctl(_HIO* h, INT32 req, VOID* val);
INT32 _pin_write(_HIO* h, const VOID* v, UINT32 sz, UINT32 n);
INT32 _pin_read(_HIO* h, VOID* v, UINT32 sz, UINT32 n);
VOID _pin_close(_HIO* h);

_HPORT* _port_open(CHAR* mode);
INT32 _port_ioctl(_HPORT* h, INT32 req, VOID* val);
INT32 _port_write(_HPORT* h, const VOID* v, UINT32 sz, UINT32 n);
INT32 _port_read(_HPORT* h, VOID* v, UINT32 sz, UINT32 n);
VOID _port_close(_HPORT* h);

_HPULSE* _pulse_open(CHAR* mode);
INT32 _pulse_ioctl(_HPULSE* h, INT32 req, VOID* val);
INT32 _pulse_write(_HPULSE* h,const VOID* v, UINT32 sz, UINT32 n);
INT32 _pulse_read(_HPULSE* h, VOID* v, UINT32 sz, UINT32 n);
VOID _pulse_close(_HPULSE* h);

_HINT* _int_open(CHAR* m);
INT32 _int_ioctl(_HINT* h, INT32 req, VOID* val);
INT32 _int_write(_HINT* h, const VOID* v, UINT32 sz, UINT32 n);
VOID _int_close(_HINT* h);

_HPWM* _pwm_open(CHAR* m);
INT32 _pwm_write(_HPWM* h,const VOID* v, UINT32 sz, UINT32 n);
INT32 _pwm_read(_HPWM* h, VOID* v, UINT32 sz, UINT32 n);
INT32 _pwm_ioctl(_HPWM* h, INT32 req, VOID* s);
VOID _pwm_close(_HPWM* h);

_H328* _328_open(CHAR* m);
INT32 _328_write(_H328* h,const VOID* v, UINT32 sz, UINT32 n);
INT32 _328_read(_H328* h, VOID* v, UINT32 sz, UINT32 n);
INT32 _328_ioctl(_H328* h, INT32 req, VOID* val);
VOID _328_close(_H328* h);

#ifdef __cplusplus
}
#endif

/*
///da testare
HDCLK ios_dclock(UINT32 pck, UINT32 pdt, UINT32 fq, BOOL syncup);
VOID ios_dclock_send(HDCLK h, BYTE* data, UINT32 szbit);
VOID ios_dclock_close(HDCLK h);

UINT32 ios_pulse(HIO h, UINT32 c);
FLOAT64 ios_dutyon(HIO h, UINT32 c);

VOID ios_lcda_send(HLCDA h, BYTE value, BYTE mode);
HLCDA ios_lcda( BOOL nibblemode,BOOL oneline,BOOL dotsize, UINT32 pinrs,  UINT32 pinrw, UINT32 pinenable,
			     UINT32 pd0, UINT32 pd1, UINT32 pd2, UINT32 pd3, 
			     UINT32 pd4, UINT32 pd5, UINT32 pd6, UINT32 pd7, BOOL begin);
VOID ios_lcda_close(HLCDA h);
VOID ios_lcda_clear(HLCDA h);
VOID ios_lcda_home(HLCDA h);
VOID ios_lcda_setcursor(HLCDA h, BYTE col, BYTE row);
VOID ios_lcda_nodisplay(HLCDA h);
VOID ios_lcda_display(HLCDA h);
VOID ios_lcda_nocursor(HLCDA h);
VOID ios_lcda_cursor(HLCDA h);
VOID ios_lcda_noblink(HLCDA h);
VOID ios_lcda_blink(HLCDA h);
VOID ios_lcda_scrolldisplayleft(HLCDA h);
VOID ios_lcda_scrolldisplayright(HLCDA h);
VOID ios_lcda_lefttoright(HLCDA h);
VOID ios_lcda_righttoleft(HLCDA h);
VOID ios_lcda_autoscroll(HLCDA h);
VOID ios_lcda_noautoscroll(HLCDA h);
VOID ios_lcda_createchar(HLCDA h, BYTE location, BYTE charmap[]) ;
VOID ios_lcda_write(HLCDA h, BYTE value);
VOID ios_lcda_print(HLCDA h, CHAR* s);
*/

#endif
