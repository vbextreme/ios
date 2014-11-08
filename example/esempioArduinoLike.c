//START BASE ARDUINO

#define OUTPUT 1
#define INPUT  0
#define HIGH 1
#define LOW 0

static BOOL EXIT_LOOP = FALSE;

#define ARDUINO_MODE void setup(void);\
					 void loop(void);\
					 int main()\
					 {\
						 initArduinoLike();\
						 setup();\
						 while ( !EXIT_LOOP )\
							loop();\
						 freeArduinoLike();\
						 return 0;\
					 }

HIOS pin[37];
HIOS ard;

typedef enum {P0,P1,P2,P3,P4,P5,P6,P7,P10,P11,P12,P13,P14,P15,P16,P17,
			  D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,A0,A1,A2,A3,A4,A5} PIN;

extern const BYTE IOS_PIN_HIGH;
extern const BYTE IOS_PIN_LOW;
extern const BYTE IOS_PWM_PAUSE;
extern const BYTE IOS_PWM_START;
extern const BYTE IOS_PWM_RESUME;

void initArduinoLike()
{
	ard_init();
	
	ios_exportall(IOS_EXPORT);
	
	int i;
	for (i = 0; i < 37; ++i)
		pin[i] = NULL;
		
	//special function
	ard = ios_open("ios/328","w");
}

void freeArduinoLike()
{
	ard_close();
	ios_exportall(IOS_UNEXPORT);
	int i;
	for (i = 0; i < 37; ++i)
		if (pin[i] ) ios_close(pin[i]);
		
	ios_close(ard);
}

int _mappin(PIN p)
{
	if (p < P0 || p > A5 ) return -1;
	static int rport[37] = { IOS_P00,IOS_P01,IOS_P02,IOS_P03,IOS_P04,IOS_P05,IOS_P06,IOS_P07,
							 IOS_P10,IOS_P11,IOS_P12,IOS_P13,IOS_P14,IOS_P15,IOS_P16,IOS_P17,
							 IOS_D00,IOS_D01,IOS_D02,IOS_D03,IOS_D04,IOS_D05,IOS_D06,IOS_D07,
							 IOS_D08,IOS_D09,IOS_D10,IOS_D11,IOS_D12,IOS_D13,
							 IOS_A00,IOS_A01,IOS_A02,IOS_A03,IOS_A04,IOS_A05};
							 
	return rport[p];
}

void pinMode(PIN p, BYTE dir)
{
	int rp = _mappin(p);
		if ( rp < 0 ) return;
	
	if (pin[p]) ios_close(pin[p]);
	
	pin[p] = ios_open("ios/pin",(dir) ? "w" : "r");
	ios_ioctl(pin[p],IOS_IOCTL_PIN_SET,&rp);
}

void digitalWrite(PIN p, BYTE v)
{
	if (p < P0 || p > A5 ) return;
	ios_write(pin[p],&v,1);
}

INT16 digitalRead(PIN p)
{
	if (p < P0 || p > A5 ) return 0;
	INT16 ret;
	ios_read(pin[p],(BYTE*)&ret,2);
	return ret;
}

INT16 analogRead(PIN p)
{
	if (p < A0 || p > A5 ) return 0;
	INT16 ret;
	ios_read(pin[p],(BYTE*)&ret,2);
	return ret;
}

void delay(INT32 t)
{
	ios_msleep(t);
}

void sleep()
{
	ios_ioctl(ard,IOS_IOCTL_328_SLEEP,NULL);
}

void wake()
{
	ios_ioctl(ard,IOS_IOCTL_328_WAKE,NULL);
}

//END 


ARDUINO_MODE

int on = 1;
int repeat = 11;

void setup()
{
        //defensive
		//defaul firmware autosleep 5m wake to sure on
		wake();
	        delay(1000); //wait 1 second because if 328 enter in sleep and in this moment send wake increase time to wake...
	//end

	pinMode(D3,OUTPUT);
}

void loop()
{
	delay(1000);
	digitalWrite(D3,on);
	on = !on;
	
	//set to end software
	if (repeat-- <= 0 ) EXIT_LOOP = TRUE;
}
