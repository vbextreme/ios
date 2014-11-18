#ifndef __IOS_H__
#define __IOS_H__

/*
 *  open:
 *  lib/dev , mode
 *  lib = "ios" default lib
 *  dev = pin,port,pwm,lcda
 *  u3l reuired su
*/ 

//testare pwm 328
//testare rwm 

#include <easytype.h>

///PWM MAX OUT 980hz
///PWM DUTY TON MIN 0.00066s
///PIN WRITE/READ MIN 0.000512s = 512us = 512000ns

#define IOS_LIB_NAME_MAX 30
#define IOS_DEV_NAME_MAX 20
#define IOS_PATH_MAX 256

#define IOS_ERR_OPEN -1
#define IOS_ERR_WRITE -2
#define IOS_ERR_READ -3
#define IOS_ERR_CLOSE -4
#define IOS_ERR_PIN -5
#define IOS_ERR_DIR -6
#define IOS_ERR_IOCTL -7
#define IOS_ERR_EXPORT -8
#define IOS_ERR_PWM -9

#define IOS_EXPORT 1
#define IOS_UNEXPORT 0

#define IOS_P_MIN 289
#define IOS_P_MAX 304

#define IOS_P00 289
#define IOS_P01 290
#define IOS_P02 291
#define IOS_P03 292
#define IOS_P04 293
#define IOS_P05 294
#define IOS_P06 295
#define IOS_P07 296
#define IOS_P10 297
#define IOS_P11 298
#define IOS_P12 299
#define IOS_P13 300
#define IOS_P14 301
#define IOS_P15 302
#define IOS_P16 303
#define IOS_P17 304
#define IOS_D00 1000
#define IOS_D01 1001
#define IOS_D02 1002
#define IOS_D03 1003
#define IOS_D04 1004
#define IOS_D05 1005
#define IOS_D06 1006
#define IOS_D07 1007
#define IOS_D08 1008
#define IOS_D09 1009
#define IOS_D10 1010
#define IOS_D11 1011
#define IOS_D12 1012
#define IOS_D13 1013
#define IOS_A00 1014
#define IOS_A01 1015
#define IOS_A02 1016
#define IOS_A03 1017
#define IOS_A04 1018
#define IOS_A05 1019

#define IOS_DIR_OUT 1
#define IOS_DIR_IN  0

#define IOS_DUTY_50 0.0
#define IOS_INFINITE 0
#define IOS_OFFSET_SLEEP 0.00015

#define IOS_INT_EXECL 1
#define IOS_INT_CBK 2

#define IOS_IOCTL_U3L_MMC       0x00
#define IOS_IOCTL_U3L_SD        0x01
#define IOS_IOCTL_U3L_TIMER     0x02
#define IOS_IOCTL_U3L_TIMER_ON  0x03
#define IOS_IOCTL_U3L_TIMER_OFF 0x04
#define IOS_IOCTL_U3L_ONESHOT   0x05
#define IOS_IOCTL_U3L_GPIO      0x06

#define IOS_IOCTL_PIN_SET     0x00

#define IOS_IOCTL_PORT_SET_0  0x00
#define IOS_IOCTL_PORT_SET_1  0x01
#define IOS_IOCTL_PORT_SET_2  0x02
#define IOS_IOCTL_PORT_SET_3  0x03
#define IOS_IOCTL_PORT_SET_4  0x04
#define IOS_IOCTL_PORT_SET_5  0x05
#define IOS_IOCTL_PORT_SET_6  0x06
#define IOS_IOCTL_PORT_SET_7  0x07
#define IOS_IOCTL_PORT_SET_A  0x08

#define IOS_IOCTL_PULSE_SET     0x00
#define IOS_IOCTL_PULSE_MODE    0x01
#define IOS_IOCTL_PULSE_US      0x02
#define IOS_IOCTL_PULSE_MODERET 0x03

#define IOS_IOCTL_INT_SET      0x00
#define IOS_IOCTL_INT_MODE     0x01
#define IOS_IOCTL_INT_MODE_SET 0x02
#define IOS_IOCTL_INT_PARAM    0x03
#define IOS_IOCTL_INT_STAT     0x04
#define IOS_IOCTL_INT_BOUNCE   0x05
#define IOS_IOCTL_INT_WAIT     0x06

#define IOS_IOCTL_PWM_WAIT       0x0
#define IOS_IOCTL_PWM_SET        0x1
#define IOS_IOCTL_PWM_SET_DUTY   0x2
#define IOS_IOCTL_PWM_SET_FQ     0x3
#define IOS_IOCTL_PWM_SET_TOUT   0x4
#define IOS_IOCTL_PWM_END_MODE   0x5
#define IOS_IOCTL_PWM_READ_RESET 0x6
#define IOS_IOCTL_PWM_ELAPSE     0x7

#define IOS_IOCTL_328_ECHO    0x00
#define IOS_IOCTL_328_SLEEP   0x01
#define IOS_IOCTL_328_OPZ     0x02
#define IOS_IOCTL_328_MAXTO   0x03
#define IOS_IOCTL_328_PRE_SET 0x04
#define IOS_IOCTL_328_MEM_STA 0x05
#define IOS_IOCTL_328_NOP     0x06
#define IOS_IOCTL_328_OPPC    0x07
#define IOS_IOCTL_328_DIR     0x08
#define IOS_IOCTL_328_OVDT    0x09
#define IOS_IOCTL_328_FREERAM 0x0A
#define IOS_IOCTL_328_WAKE    0x0B

#define IOS_328_PRE_1         0x01
#define IOS_328_PRE_8         0x02
#define IOS_328_PRE_32        0x03
#define IOS_328_PRE_64        0x04
#define IOS_328_PRE_128       0x05
#define IOS_328_PRE_256       0x06
#define IOS_328_PRE_1024      0x07

#define IOS_IOCTL_LCDA_INIT        0x00
#define IOS_IOCTL_LCDA_CLEAR       0x01
#define IOS_IOCTL_LCDA_HOME	       0x02
#define IOS_IOCTL_LCDA_CURSOR      0x03
#define IOS_IOCTL_LCDA_NOTDISPLAY  0x04
#define IOS_IOCTL_LCDA_DISPLAY     0x05
#define IOS_IOCTL_LCDA_NOCURSOR    0x06
#define IOS_IOCTL_LCDA_CURSORON    0x07
#define IOS_IOCTL_LCDA_NOBLINK     0x08
#define IOS_IOCTL_LCDA_BLINK       0x09
#define IOS_IOCTL_LCDA_SCROLLDSPSX 0x0A
#define IOS_IOCTL_LCDA_SCROLLDSPDX 0x0B
#define IOS_IOCTL_LCDA_SXTODX      0x0C
#define IOS_IOCTL_LCDA_DXTOSX      0x0D
#define IOS_IOCTL_LCDA_AUTOSCROL   0x0E
#define IOS_IOCTL_LCDA_NOAUTOSCROL 0x0F
#define IOS_IOCTL_LCDA_NEWCHAR     0x10

#define IOS_328_OPZ_DBG          0x01
#define IOS_328_OPZ_ASL          0x02
#define IOS_328_OPZ_IOPC_RUN     0x04
#define IOS_328_OPZ_OVDT         0x08

#define IAL_CODE_MAX 5000
#define IAL_BYTECODE_MAX 512
#define IAL_ERR_STRING -1
#define IAL_ERR_FILE   -2
#define IAL_ERR_DEFINE -3
#define IAL_ERR_IFDEF  -4
#define IAL_ERR_MULTIPLEIFDEF -6
#define IAL_ERR_MAKE -7

typedef struct __HIOS* HIOS;

typedef VOID (*IOSINT)(VOID* par);

#ifdef __cplusplus
extern "C" {
#endif

FLOAT64 ios_clock_get();
UINT32 ios_mclock_get();
UINT32 ios_uclock_get();
UINT32 ios_nclock_get();
VOID ios_msleep(UINT32 ms);
VOID ios_usleep(UINT32 us);
VOID ios_nsleep(UINT32 nano);

BOOL ios_exported(UINT32 pin);
INT32 ios_export(UINT32 pin, UINT32 mode);
INT32 ios_exportall(UINT32 mode);

INT32 ios_dir_write(UINT32 pin, UINT32 d);
INT32 ios_dir_read(UINT32 pin);

VOID ard_init();
VOID ard_close();
INT32 i328_make(UINT32* offset, BYTE* bytecode, CHAR* code);

HIOS ios_open(CHAR* t,CHAR* m);
INT32 ios_write(HIOS h,const VOID* v, UINT32 sz, UINT32 n);
INT32 ios_read(HIOS h, VOID* v, UINT32 sz, UINT32 n);
INT32 ios_ioctl(HIOS h, INT32 req, VOID* v);
VOID ios_close(HIOS h);

#ifdef __cplusplus
}
#endif

#endif
