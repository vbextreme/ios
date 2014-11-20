#include "ref.h"

//MEMORIA RIMASTA APPROSSIMATIVA :< 488

///OPZIONI
byte opz;
byte dissy;
int o_tosleep;
int o_maxto;
unsigned long o_overflowdata;

// PROTO
// cmd p0 p1 p2
byte _pc[MAX_CMD];
byte _p0[MAX_CMD];
byte _p1[MAX_CMD];
byte _p2[MAX_CMD];
byte _pw,_pr;



// PROTO RESP
byte p_res[4];

#define cmd_pop(C,P0,P1,P2) C = _pc[_pr]; P0 = _p0[_pr] ; P1 = _p1[_pr] ; P2 = _p2[_pr++]

// SUPPORT PWM
unsigned long pwm_dton[20];
unsigned long pwm_dtof[20];
unsigned long pwm_fq[20];
unsigned long pwm_clk[20];
unsigned long pwm_tout[20];
unsigned long pwm_enable;

//OFFLINE
byte mem[MAX_MEM];
unsigned int imem;
unsigned int oppc;
unsigned int reg[MAX_REG];


int cl_isempty()
{
    return ( _pr == _pw ) ? 1 : 0;
}

int cl_isfull()
{
    if ( _pw + 1 >= MAX_CMD )
        return ( 0 == _pr ) ? 1 : 0;
    else
        return ( _pw + 1 == _pr ) ? 1 : 0;
}

int _freeram()
{
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup()
{
  opz = OPZ_DBG | OPZ_ASL | OPZ_OVDT;
  o_tosleep = o_maxto = TOSLEEP;
  o_overflowdata = 0;
  dissy = 0;
  _pr = 0;
  _pw = 0;
  imem = 0;
  oppc = 0;
  unsigned int i;
  for (i = 0; i < MAX_REG; ++i) reg[i] = 0;
  for (i = 0; i < MAX_MEM; ++i) mem[i] = CMD_NOP;
  
  pwm_enable = 0;

  Serial.begin(9600);
  pinMode(PIN_DBG,OUTPUT);
}

void loop()
{
    byte pc,p0,p1,p2;
    int gp;
    unsigned long gpl;
    //ONDBG(ON);
    //byte d;
    
    while (1)
    {
        if ( (opz & OPZ_OVDT) && cl_isfull()  )
            ++o_overflowdata;
        
        while ( !cl_isfull() && ( Serial.available() > 3 ) )
        {   
            _pc[_pw] = Serial.read();
            _p0[_pw] = Serial.read();
            _p1[_pw] = Serial.read();
            _p2[_pw++] = Serial.read();
            if ( _pw == MAX_CMD ) _pw = 0;
        }
        
        //controllo comandi se vuoto attesa fino a sleep se impostato
        if ( cl_isempty() )
        {
            //interprete
            if ( opz & OPZ_IOPC_RUN )
            {
                if ( opz & OPZ_SYNC ) {opz &= ~OPZ_SYNC; dissy = 1;} 
                pc = mem[oppc++];
                if ( oppc >= MAX_MEM ) oppc = 0;
                p0 = mem[oppc++];
                if ( oppc >= MAX_MEM ) oppc = 0;
                p1 = mem[oppc++];
                if ( oppc >= MAX_MEM ) oppc = 0;
                p2 = mem[oppc++];
                if ( oppc >= MAX_MEM ) oppc = 0;
                //ONDBG(READ)
            }
            else if ( opz & OPZ_ASL )
            {
                delay(WAITCMD);
                if ( o_tosleep-- == 0 )
                {
                    o_tosleep = o_maxto;
                    ard_sleep();
                }
                continue;
            }
            else
            {
              continue;
            }
        }
        else
        {
            if ( dissy ) {opz &= OPZ_SYNC; dissy = 1;} 
            pc = _pc[_pr];
            p0 = _p0[_pr];
            p1 = _p1[_pr];
            p2 = _p2[_pr++];
            if ( _pr == MAX_CMD ) _pr = 0;
        }
        
        //esecuzione comando
        switch( pc )
        {
            case CMD_NOP: break;
          
            case CMD_SLEEP: ard_sleep(); break;
          
            case CMD_ECHO:
                cmd_resp(CMD_ECHO,p0,p1,p2);
                cmd_write();
            break;
            
            case CMD_OPZ: 
                if ( p1 ) 
                    opz = reg[p0]; 
                else  
                    opz = p0; 
            break; 
            
            case CMD_GPZ: 
                if ( p1 ) 
                    reg[p0] = opz; 
                else  
                {
                    cmd_resp(CMD_GPZ,opz,0,0);
                    cmd_write();
                } 
            break; 
            
            
            case CMD_MAXTO:
                if ( p2 ) 
                    o_maxto = reg[p0];
                else
                    o_maxto = ( p1 << 8 ) | p0; 
            break; 
            
            case CMD_OVDT: cmd_resp(CMD_OVDT,(byte)(o_overflowdata),(byte)(o_overflowdata >> 8),(byte)(o_overflowdata >> 16)); break;
            
            case CMD_PIN_DIR: 
                if ( p2 )
                    pinMode(reg[p0],reg[p1]); 
                else
                    pinMode(p0,p1); 
            break;

            case CMD_PIN_WRITE: 
                if ( p2 )
                    digitalWrite(reg[p0],reg[p1]); 
                else
                    digitalWrite(p0,p1); 
            break;
            
            case CMD_PIN_READ:
                if ( p2 )
                {
                    if ( reg[p0] >= 0 && reg[p0] < 14 )
                    {
                      reg[p1] = digitalRead(reg[p0]);
                    }
                    else if ( reg[p0] > 13 && reg[p0] < 20 )
                    {
                        reg[1] = analogRead(reg[p0] - 14);
                    }
                }
                else 
                {
                    if ( p0 >= 0 && p0 < 14 )
                    {
                        cmd_resp(CMD_PIN_READ,p0,digitalRead(p0),0);
                        cmd_write();
                    }
                    else if ( p0 > 13 && p0 < 20 )
                    {
                        p0 -= 14;
                        gp = analogRead(p0);
                        cmd_resp(CMD_PIN_READ, p0 + 14,(byte)gp,(byte)(gp >> 8));
                        cmd_write();
                    }
                }
            break; 
            
            
            case _CMD_PWM_OUT:
                if ( pwm_tout[p0] - millis() >= pwm_tout[p0] ) pwm_enable &= !(1 << p0);
                if ( !(pwm_enable & (1 << p0)) ) break;
                gpl = micros();
                if ( p1 )
                {
                    if ( pwm_clk[p0] + pwm_dton[p0] >= gpl )
                    {
                        p1 = 0;
                        digitalWrite(p0,0);
                        pwm_clk[p0] = micros();
                    }
                }
                else
                {
                    if ( pwm_clk[p0] + pwm_dtof[p0] >= gpl)
                    {
                        p1 = 1;
                        digitalWrite(p0,1);
                        pwm_clk[p0] = micros();
                    }
                }
                cmd_add(_CMD_PWM_OUT, p0, p1,0);
            break;
            
            
            case CMD_PWM_DISABLE:
                if ( p1 )
                    pwm_enable &= !(1 << reg[p0]);                    
                else
                    pwm_enable &= !(1 << p0);
            break;
            
            
            case CMD_PWM_ENABLE:
                if ( p1 ) p0 = reg[p0];
                
                pwm_enable |= ( 1 << p0 );
                
                gpl = ( 1000000 / pwm_fq[p0]) / 2;
                pwm_dton[p0] = ( gpl * pwm_dton[p0]) / 256;
                pwm_dtof[p0] =  gpl - pwm_dton[p0];
                digitalWrite(p0,1);
                pwm_clk[p0] = micros();
                cmd_add(_CMD_PWM_OUT, p0, 1, 0)
            break;
            
            case CMD_PWM_DT_SET:
                if ( p2 )
                {
                    p0 = reg[p0];
                    p1 = reg[p1];
                }
                
                if ( pwm_enable & (1 << p0) )
                {
                    gpl = ( 1000000 / pwm_fq[p0]) / 2;
                    pwm_dton[p0] =( gpl * p1) / 256;
                    pwm_dtof[p0] =  gpl - pwm_dton[p0];
                }
                else
                    pwm_dton[p0] = p1;
            break;
            
            case CMD_PWM_FQ_SET:
                if ( p0 > 99 )
                    pwm_fq[reg[p0-100]] = reg[p1]; 
                else
                    pwm_fq[p0] = ( p2 << 8 ) | p1; 
            break;
                
            case CMD_PWM_TOUT_SET: 
                if ( p0 > 99 )            
                {
                    pwm_tout[reg[p0 - 100]] = reg[p1] + millis(); 
                }
                else
                    pwm_tout[p0] = (( p2 << 8 ) | p1) + millis(); 
            break;
            
            case CMD_PULSED_OUT:
                if ( p0 > 99 )            
                {
                    p0 = reg[p0 - 100]; 
                    gp = reg[p1];
                }
                else
                    gp = ( p2 << 8 ) | p1;
                
                digitalWrite(p0,0);
                delayMicroseconds(gp);
                digitalWrite(p0,1);
            break;
            
            case CMD_PULSEU_OUT:
                if ( p0 > 99 )            
                {
                    p0 = reg[p0 - 100]; 
                    gp = reg[p1];
                }
                else
                    gp = ( p2 << 8 ) | p1;
                
                digitalWrite(p0,1);
                delayMicroseconds(gp);
                digitalWrite(p0,0);
            break;
            
            case CMD_PULSEF_IN:
                if ( p2 )            
                {
                    p0 = reg[p0]; 
                    p1 = reg[p1];
                }
                
                while( digitalRead(p0) == p1 );
                while( digitalRead(p0) != p1 );
                
                gpl = micros();
                while( digitalRead(p0) == p1 );
                while( digitalRead(p0) != p1 );
                gpl = micros() - gpl;
                if ( p2 )
                    reg[p2] = (unsigned int) gpl;
                else
                {
                    cmd_resp(CMD_PULSEF_IN, (byte)gpl, (byte)(gpl >> 8), (byte)(gpl >> 16));
                    cmd_write();
                }
            break;
            
            case CMD_PULSEC_IN:
                if ( p2 )            
                {
                    p0 = reg[p0]; 
                    p1 = reg[p1];
                }
                
                while( digitalRead(p0) == p1 );
                while( digitalRead(p0) != p1 );
                
                gpl = micros();
                while( digitalRead(p0) == p1 );
                gpl = micros() - gpl;
                if ( p2 )
                    reg[p2] = (unsigned int) gpl;
                else
                {
                    cmd_resp(CMD_PULSEF_IN, (byte)gpl, (byte)(gpl >> 8), (byte)(gpl >> 16));
                    cmd_write();
                }
            break;
            
            case CMD_PRE_SET: 
                if ( p2 )
                    ard_pwm_fq(reg[p0],reg[p1]); 
                else
                    ard_pwm_fq(p0,p1); 
            break;
            
            case CMD_PWM_AUTO:
                if ( p2 ) 
                    analogWrite(reg[p0],reg[p1]);
                else
                    analogWrite(p0,p1);
            break;
            
            case CMD_MEM_STA: imem = ( p1 << 8 ) | p0; break;
            case CMD_MEM_WRITE: 
                mem[imem++] = p0; 
                if (imem == MAX_MEM) imem = 0;
            break;
            case CMD_MEM_READ: 
                cmd_resp(CMD_MEM_READ, mem[imem], 0, 0);
                if ( ++imem == MAX_MEM ) imem = 0;
            break; 
            
            case CMD_OPPC: oppc = ( p1 << 8 ) & p0; break;
            
            case CMD_RR : reg[p0] = reg[p1]; break;
            case CMD_RI : reg[p0] = ( p2 << 8 ) | p1; break;
            case CMD_RM : reg[p0] = mem[reg[p1]]; break;
            case CMD_MR : mem[reg[p0]] = reg[p1]; break;            
            
            case CMD_ADD : reg[p0] = reg[p1] + reg[p2]; break;
            case CMD_DIF : reg[p0] = reg[p1] - reg[p2]; break;
            case CMD_MUL : reg[p0] = reg[p1] * reg[p2]; break;      
            case CMD_DIV : reg[p0] = reg[p1] / reg[p2]; break;      
            case CMD_SHL : reg[p0] = reg[p1] << reg[p2]; break;      
            case CMD_SHR : reg[p0] = reg[p1] >> reg[p2]; break;      
            case CMD_OR  :  reg[p0] = reg[p1] | reg[p2]; break;      
            case CMD_AND :  reg[p0] = reg[p1] & reg[p2]; break;      
            case CMD_XOR :  reg[p0] = reg[p1] ^ reg[p2]; break;      
            case CMD_NOT :  reg[p0] = !reg[p1]; break;      
            
            case CMD_JMP : oppc = reg[p0]; break;
            case CMD_JMI : if ( reg[p1] < reg[p2] ) oppc = reg[p0]; break;
            case CMD_JMA : if ( reg[p1] > reg[p2] ) oppc = reg[p0]; break;
            case CMD_JEQ : if ( reg[p1] == reg[p2] ) oppc = reg[p0]; break;
            case CMD_JAN : if ( reg[p1] && reg[p2] ) oppc = reg[p0]; break;
            case CMD_JOR : if ( reg[p1] || reg[p2] ) oppc = reg[p0]; break;
            case CMD_JNO : if ( !reg[p1] ) oppc = reg[p0]; break;
            
            case CMD_PUSH : mem[reg[p0]--] = reg[p1]; break;
            case CMD_POP : reg[p1] = mem[++reg[p0]];  break;        
            
            case CMD_CALL:
                mem[reg[p0]--] =(byte)oppc;
                mem[reg[p0]--] =(byte)(oppc << 8);
                oppc = ( p2 << 8 ) & p1;
            break;
            case CMD_RET:
                oppc = mem[++reg[p0]] << 8;
                oppc |= mem[++reg[p0]];
            break;                
            
            case CMD_MDELAY:
                delay(reg[p0]);
            break;
            
            case CMD_UDELAY:
                delayMicroseconds(reg[p0]);
            break;
            
            case CMD_FREERAM:
                gp = _freeram();
                cmd_resp(CMD_FREERAM,(byte)gp,(byte)(gp>>8),0);
                cmd_write();
            break;
            
            default: 
                
            break; 
        }//SWITCH
        
        if ( opz & OPZ_SYNC )
        {
            cmd_resp(CMD_SYNCCLK,0,0,0);
            cmd_write();            
        }
    }//for erver
}//main

