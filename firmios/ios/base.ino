#include "ref.h"

extern byte o_dbg;

//divisore:= 1 = 1, 2 = 8, 3 = 32, 4 = 64, 5 = 128, 6 = 256, 7 = 1024;
void ard_pwm_fq(byte pin, byte divisor) 
{
    if ( pin != 9 || pin != 10 || pin != 3 || pin != 11 ) return;
    
    byte mode;
    
    if( pin == 9 || pin == 10 ) 
    {
        switch(divisor) 
        {
            case 1: mode = 0x01; break;
            case 2: mode = 0x02; break;
            case 3: return;
            case 4: mode = 0x03; break;
            case 5: return;
            case 6: mode = 0x04; break;
            case 7: mode = 0x05; break;
            default: return;
        }

        TCCR1B = (TCCR1B & 0b11111000) | mode;
    
    } 
    else if ( pin == 3 || pin == 11 ) 
    {
        switch(divisor) 
        {
            case 1: mode = 0x01; break;
            case 2: mode = 0x02; break;
            case 3: mode = 0x03; break;
            case 4: mode = 0x04; break;
            case 5: mode = 0x05; break;
            case 6: mode = 0x06; break;
            case 7: mode = 0x07; break;
            default: return;
        }
        TCCR2B = (TCCR2B & 0b11111000) | mode;
    }
}


void ard_dbg_led(byte nblk, int dyms)
{
  while( nblk-- )
  {
    digitalWrite(PIN_DBG,HIGH);
    delay(dyms);
    digitalWrite(PIN_DBG,LOW);
    delay(dyms);
  }
}

void ard_sleep()
{
  ONDBG(SLEEP);
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();
  sleep_mode();
  sleep_disable();
  power_all_enable();
  ONDBG(WAKE);
}

