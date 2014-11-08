#ifndef __REF_H__
#define __REF_H__

#include <avr/power.h>
#include <avr/sleep.h>

#define ERR_UNDEFCMD 0
#define ERR_PIN      1
#define ERR_MEM      2

#define TOSLEEP  30000
#define WAITCMD     10

#define PIN_DBG 13
#define MAX_CMD 100
#define MAX_MEM 512
#define MAX_REG 16


#define OPZ_DBG          0x01
#define OPZ_ASL          0x02
#define OPZ_IOPC_RUN     0x04
#define OPZ_OVDT         0x08


#define ONRES(C,P0,P1,P2) if ( opz & OPZ_RES ) { cmd_resp(C,P0,P1,P2); cmd_write();}

#define ONDBG(N) if( opz & OPZ_DBG ) {ard_dbg_led(DBG_I_ ## N, DBG_T_ ## N);}
#define DBG_T_SLEEP 500
#define DBG_I_SLEEP 6
#define DBG_T_WAKE  1000
#define DBG_I_WAKE  1
#define DBG_T_ON    500
#define DBG_I_ON    10
#define DBG_T_ERR   150
#define DBG_I_ERR   5
#define DBG_T_READ  100
#define DBG_I_READ  1
#define DBG_T_EXE   500
#define DBG_I_EXE   1


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



typedef unsigned char byte; //???

#define cmd_add(C,P0,P1,P2) if( !cl_isfull() ) \
                            {\
                                _pc[_pw] = C;\
                                _p0[_pw] = P0;\
                                _p1[_pw] = P1;\
                                _p2[_pw++] = P2;\
                                if ( _pw == MAX_CMD ) _pw = 0;\
                            }
                            
#define cmd_resp(C,P0,P1,P2) p_res[0] = C; p_res[1] = P0; p_res[2] = P1; p_res[3] = P2
#define cmd_write() Serial.write(p_res,4)

void ard_pwm_fq(byte pin, byte divisor) ;
void ard_dbg_led(byte nblk, int dyms);
void ard_sleep();

#endif

