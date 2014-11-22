#ifdef _APP

#include "main.h"
#include <stdarg.h>
#include <easythread.h>
#include <math.h>
#include <time.h>
#include <easyconsole.h>

/*
void printcode(CHAR* s, BYTE* v)
{
	printf("%s --- 0x%X 0x%X 0x%X 0x%X\n",s,v[0],v[1],v[2],v[3]);
}

int example_i328()
{
	
	//test icode
	CHAR* code = "ri %1,1                ; out\n"
	             "ri %2,0                ; zeroval\n"
				 "ri %3,13               ; on board led\n"
				 "pindir %3,%1           ; set direction\n"
				 "ri %4,500      0       ; time\n"
				 "MAIN: pinwrite %3,%2 ;\n"
				 "      mdelay %4        ; wait\n"
				 "      pinwrite %3,%1 ;\n"
				 "      mdelay %4        ; wait\n"
				 "      jmp MAIN         ;loop\n";
	
	//make to bytecode
	CHAR pcode[IAL_CODE_MAX];
		strcpy(pcode,code);
		
	BYTE icode[IAL_BYTECODE_MAX];
	UINT32 offset = 0;
	
	INT32 ret = i328_make(&offset,icode,pcode);
		if ( ret ) {puts("error make");return ret;}
	
	printcode("ri %%1,1      ",&icode[0]);
	printcode("ri %%2,0      ",&icode[4]);
	printcode("ri %%3,13     ",&icode[4]);
	printcode("pindir %%3,%%1",&icode[8]);
	printcode("ri %%4,500    ",&icode[12]);
	printcode("pinwrite %3,%2",&icode[16]);
	printcode("mdelay %4     ",&icode[20]);
	printcode("pinwrite %3,%1",&icode[24]);
	printcode("mdelay %4     ",&icode[28]);
	printcode("jmp MAIN      ",&icode[32]);
	
	return 0;
	
	//send code to 328
	HIOS h = ios_open("ios/328","w");
	INT32 mempos = 0;
	ios_ioctl(h, IOS_IOCTL_328_MEM_STA, &mempos);
	ios_write(h,icode,offset);
	
	//set address to start:
	ios_ioctl(h, IOS_IOCTL_328_OPPC, &mempos);
	//start interpretate code:
	BYTE opz = IOS_328_OPZ_IOPC_RUN | IOS_328_OPZ_OVDT;
	ios_ioctl(h, IOS_IOCTL_328_OPZ, &opz);
	
	ios_msleep(5000);
	//stop
	opz = IOS_328_OPZ_ASL | IOS_328_OPZ_DBG | IOS_328_OPZ_OVDT;
	ios_ioctl(h, IOS_IOCTL_328_OPZ, &opz);
	
	ios_close(h);
	return 0;
}
*/

VOID printbit(BYTE v)
{
	INT32 i;
	for ( i = 7; i >= 0; --i )
		printf("%d", (v >> i) & 0x1 );
}

VOID buttonclick(VOID* p)
{
	printf("Button press\n");
	con_flush();
}

int main() 
{
	//ard_init();
	/*
	ios_exportall(IOS_EXPORT);
	HIOS t = ios_open("ios/int","r");
	UINT32 pin = IOS_P17;
	INT32 ior;
	if ( (ior = ios_ioctl(t,IOS_IOCTL_INT_SET,&pin) ) ) {printf("ERROR iOCTL p:%d\n",ior);return 0;}
	pin = IOS_INT_EXECL;
	if ( ios_ioctl(t,IOS_IOCTL_INT_MODE,&pin) ) {puts("ERROR iOCTL m");return 0;}
	CHAR* cmd = "ls";
	if ( ios_ioctl(t,IOS_IOCTL_INT_MODE_SET,cmd) ) {puts("ERROR iOCTL ms");return 0;}
	pin = 100;
	if ( ios_ioctl(t,IOS_IOCTL_INT_BOUNCE,&pin) ) {puts("ERROR iOCTL bopu");return 0;}
	
	BYTE st = 1;
	ios_write(t,&st,1,1);
	
	ios_ioctl(t,IOS_IOCTL_INT_WAIT,NULL);
	*/
	
	/*
	HIOS t = ios_open("lightios/led","w");
		if ( !t ) {puts("Error open library"); return 0;}
	
	INT32 ret = IOS_P02;
	if ( (ret = ios_ioctl(t,IOS_IOCTL_PIN_SET,&ret)) ) printf("Error ioctl:%d\n",ret);
	ios_msleep(1000);
	
	ret = 255;
	ios_write(t,&ret,1,1);
	ios_msleep(1000);
	
	ret = 100;
	ios_write(t,&ret,1,1);
	ios_msleep(1000);
	
	ret = 55;
	ios_write(t,&ret,1,1);
	ios_msleep(1000);
	
	ret = 0;
	ios_write(t,&ret,1,1);
	ios_msleep(1000);
	
	ios_close(t);
	*/
	
	/*
	ios_exportall(IOS_EXPORT);
	
	HIOS h = ios_open("ios/pwm","w");
	UINT32 val = IOS_P00;
	ios_ioctl(h, IOS_IOCTL_PWM_SET, &val);
	val = 100;
	ios_ioctl(h, IOS_IOCTL_PWM_SET_FQ, &val);
	val = 0;
	ios_ioctl(h, IOS_IOCTL_PWM_SET_DUTY, &val);
	val = 0;
	ios_ioctl(h, IOS_IOCTL_PWM_SET_TOUT, &val);
	val = 1;
	ios_ioctl(h, IOS_IOCTL_PWM_END_MODE, &val);
	BYTE b = 1;
	ios_write(h,&b,1,1);
	
	puts("off");
	ios_msleep(1000);
	
	for ( val = 0; val < 256; ++val)
	{
		ios_ioctl(h, IOS_IOCTL_PWM_SET_DUTY, &val);
		ios_msleep(50);
		printf("%d\n",val);
		fflush(stdout);
	}
		
	ios_close(h);
	ios_msleep(3000);
	
	return 0;
	*/
	
	HIOS h = ios_open("lightios/rgb","w");
	UINT32 val = 1;
	ios_ioctl(h,0x03,&val);
	val = IOS_P00;
	ios_ioctl(h, 0x00, &val);
	val = IOS_P01;
	ios_ioctl(h, 0x01, &val);
	val = IOS_P02;
	ios_ioctl(h, 0x02, &val);
	
	puts("rr");
	UINT32 col = 0x00FF0000;
	ios_write(h,&col,4,1);
	ios_msleep(1000);
	
	puts("gg");
	col = 0x0000FF00;
	ios_write(h,&col,4,1);
	ios_msleep(1000);
	
	puts("bb");
	col = 0x000000FF;
	ios_write(h,&col,4,1);
	ios_msleep(1000);
	
	
	ios_close(h);
	ios_msleep(5000);
	
	return 0;
	
	/*
	
	
	
	INT32 coud = 10;
	
	while (coud--)
	{
		con_cls();
		con_gotorc(1,1);
		printf("[%d]  ",coud);
		con_flush();
		
		ios_read(t,(BYTE*)&ret,8);
		printf("%lf\n",ret);
		con_flush();
		
		ios_msleep(1000);
	}
	*/
	//ios_close(t);
	
	//terminate comunication 328
	//ard_close();
	
	return 0;
}

//printf("%d\n", example_i328());
/*
VOID itob(CHAR* b, UINT32 v, INT32 sz)
{
	for ( --sz; sz >= 0; --sz)
		*b++ = ((v >> sz) & 1) + '0';
	*b = '\0';
}


int mth_random(int n)
{
    return (rand() % n);
}



int example_pinout(INT32* pin)
{
	INT32 i;
	
	//PIN TEST
	puts("8 PIN TEST:");
	fflush(stdout);
	
	//init
	HIOS h[8];
	for ( i = 0; i < 8;	++i)
	{
		//export pin
		if (!ios_exported(pin[i]) && ios_export(pin[i],IOS_EXPORT) ) { puts("Error to export, check load module"); return 0;}
		//open pin
		if ( !(h[i] = ios_open("ios/pin","w")) ) { puts("Error to open pin"); return 0;}
		//assign pin
		if ( ios_ioctl(h[i], IOS_IOCTL_PIN_SET, &pin[i]) ) { puts("Error to set pin"); return 0;}	
		//all down
		if ( !ios_write(h[i],&IOS_PIN_LOW,1) ) { puts("Error to write pin"); return 0;}	
	}
	
	//example use
	INT32 sl;
	
	for ( sl = 500; sl > 50; sl -= 50)
	{
		for ( i = 1; i < 8;	++i)
		{
			ios_write(h[i],&IOS_PIN_HIGH,1);
			ios_msleep(sl);
			ios_write(h[i],&IOS_PIN_LOW,1);
		}
		for ( i = 6; i >= 0; --i)
		{
			ios_write(h[i],&IOS_PIN_HIGH,1);
			ios_msleep(sl);
			ios_write(h[i],&IOS_PIN_LOW,1);
		}
	}		
	
	//close
	for ( i = 0; i < 8;	++i)
		ios_close(h[i]);
	
	//PORT TEST
	puts("PORT TEST:");
	fflush(stdout);
	
	//open port
	HIOS port = ios_open("ios/port","w");
		if ( !port ) { puts("Error to open port"); return 0;}
	
	//init all pin
	if ( ios_ioctl(port,IOS_IOCTL_PORT_SET_A,pin)) { puts("Error to set port"); return 0;}
	
	//example equal 8 pin
	BYTE val = 0x80;
	
	ios_write(port,&val,1);
	
	for ( sl = 500; sl > 50; sl -= 50)
	{
		for ( i = 1; i < 8;	++i)
		{
			ios_write(port,&val,1);
			ios_msleep(sl);
			val = val >> 1;
		}
		for ( i = 7; i > 0; --i)
		{
			ios_write(port,&val,1);
			ios_msleep(sl);
			val = val << 1;
		}
	}		
	
	//close
	ios_write(port,&IOS_PIN_LOW,1);
	ios_close(port);

	//PULSE TEST
	puts("8 PULSE TEST:");
	fflush(stdout);
	
	//init
	for ( i = 0; i < 8;	++i)
	{	
		//open
		if ( !(h[i] = ios_open("ios/pulse","w")) ) { puts("Error to open"); return 0;}
		//set pin
		if ( ios_ioctl(h[i], IOS_IOCTL_PULSE_SET, &pin[i]) ) { puts("Error to set pin"); return 0;}	
		//set time pulse
		sl = 400;
		if ( ios_ioctl(h[i], IOS_IOCTL_PULSE_US, &sl) ) { puts("Error to set pin"); return 0;}	
		//set mode pulse 
		sl = 0;
		if ( ios_ioctl(h[i], IOS_IOCTL_PULSE_MODE, &sl) ) { puts("Error to set pin"); return 0;}	
	}
	
	//example use
	for ( sl = 0; sl < 50; ++sl)
		ios_write(h[mth_random(8)],&IOS_PIN_HIGH,1);
	
	
	//close
	for ( i = 0; i < 8;	++i)
		ios_close(h[i]);
	
	//PWM TEST
	puts("8 PWM TEST:");
	fflush(stdout);
	
	//init
	for ( i = 0; i < 8;	++i)
	{	
		//open
		if ( !(h[i] = ios_open("ios/pwm","w")) ) { puts("Error to open"); return 0;}
		//set pin
		if ( ios_ioctl(h[i], IOS_IOCTL_PWM_SET, &pin[i]) ) { puts("Error to set pin"); return 0;}	
		//set frequency
		sl = 1;//hz
		if ( ios_ioctl(h[i], IOS_IOCTL_PWM_SET_FQ, &sl) ) { puts("Error to set fq"); return 0;}	
		//set duty
		sl = 127;//0-256
		if ( ios_ioctl(h[i], IOS_IOCTL_PWM_SET_DUTY, &sl) ) { puts("Error to set dt"); return 0;}	
		//togle auto off pwm
		sl = 0;
		if ( ios_ioctl(h[i], IOS_IOCTL_PWM_SET_TOUT, &sl) ) { puts("Error to set tout"); return 0;}	
		//set pin low wend pwm is closed
		if ( ios_ioctl(h[i], IOS_IOCTL_PWM_END_MODE, &sl) ) { puts("Error to set emode"); return 0;}	
	}
	
	//example start all pwm from 1hz to 100hz duty 50%
	
	for ( i = 0; i < 8; ++i)
		ios_write(h[i],&IOS_PWM_START,1);
	
	for ( sl = 1; sl < 101; sl += 5)
	{
		for ( i = 0; i < 8; ++i)
			ios_ioctl(h[i],IOS_IOCTL_PWM_SET_FQ,&sl);
		ios_msleep(400);
	}
	
	//set duty 100% and pull down to 0
	for ( sl = 255; sl > 0; sl -= 5)
	{
		for ( i = 0; i < 8; ++i)
			ios_ioctl(h[i],IOS_IOCTL_PWM_SET_DUTY,&sl);
	}
	
	//close
	for ( i = 0; i < 8; ++i)
		ios_close(h[i]);
		
	return 0;
}


int example_328()
{
	HIOS h = ios_open("ios/328","w");
	
	//pwm set pinout
	BYTE dcom[2] = {3,1};
	ios_ioctl(h, IOS_IOCTL_328_DIR, dcom);
	dcom[0] = 11;
	ios_ioctl(h, IOS_IOCTL_328_DIR, dcom);
	
	//pwm auto prescaler 1 = 1, 2 = 8, 3 = 32, 4 = 64, 5 = 128, 6 = 256, 7 = 1024;
	dcom[1] = 7;
	ios_ioctl(h, IOS_IOCTL_328_PRE_SET,dcom);
	dcom[0] = 3;
	ios_ioctl(h, IOS_IOCTL_328_PRE_SET,dcom);
	
	//pwm out
	INT32 i;
	for ( i = 255; i >= 0; --i)
	{
		dcom[0] = 3;
		dcom[1] = i;
		ios_write(h,dcom,2);
		dcom[0] = 11;
		dcom[1] = i;
		ios_write(h,dcom,2);
	}
	
	//sleep
	ios_ioctl(h, IOS_IOCTL_328_SLEEP,NULL);
	ios_close(h);
	return 0;
}


INT32 test8led(INT32* p)
{
	INT32 i;
	for (i = 0; i < 8; ++i)
		if ( !ios_exported(p[i]) ) ios_export(p[i],IOS_EXPORT);
	
	HIOS led;
	
	for ( i = 0; i < 8; ++i)
	{
		led = ios_open("ios/pin","w");
		ios_ioctl(led,IOS_IOCTL_PIN_SET,&p[i]);
		
		ios_write(led,&IOS_PIN_HIGH,1);
		ios_msleep(400);
		ios_write(led,&IOS_PIN_LOW,1);
		
		ios_close(led);
	}
	
	return 0;
}

INT32 example_police(INT32* p)
{
	INT32 i;
	for (i = 0; i < 8; ++i)
		if ( !ios_exported(p[i]) ) ios_export(p[i],IOS_EXPORT);
	
	HIOS led[8];
	
	for ( i = 0; i < 8; ++i)
	{
		led[i] = ios_open("ios/pin","w");
		ios_ioctl(led[i],IOS_IOCTL_PIN_SET,&p[i]);
		
		ios_write(led[i],&IOS_PIN_HIGH,1);
		ios_msleep(300);
		ios_write(led[i],&IOS_PIN_LOW,1);
	}
	
	INT32 ir,t,k;
	for ( ir = 0; ir < 50; ++ir )
	{
		for ( t = 1; t < 5; ++t)
		{
			for ( k = 0; k < 4; ++k)
			{
				ios_write(led[k],&IOS_PIN_HIGH,1);
				ios_msleep(8);
			}
			for ( k = 0; k < 4; ++k)
			{
				ios_write(led[k],&IOS_PIN_LOW,1);
				ios_msleep(4);
			}
			
		}
		
		ios_msleep(30);
		
		for ( t = 1; t < 5; ++t)
		{
			for ( k = 4; k < 8; ++k)
			{
				ios_write(led[k],&IOS_PIN_HIGH,1);
				ios_msleep(8);
			}
			for ( k = 4; k < 8; ++k)
			{
				ios_write(led[k],&IOS_PIN_LOW,1);
				ios_msleep(4);
			}
		}	
		ios_msleep(30);
	}
	
	for ( k = 0; k < 8; ++k)
		ios_write(led[k],&IOS_PIN_LOW,1);
	
	return 0;
}


int main(int argc, char **argv)
{	
	
	ard_init();
		
		
	HIOS h = ios_open("ios/328","w");
		if ( !h ) {puts("error open");return 0;}
	
	INT32 sz;
	
	if ( ios_ioctl(h,IOS_IOCTL_328_FREERAM,&sz) ) puts("error ioctl");
	
	printf("memfree:%d\n",sz);
	
	ios_close(h);
	
	ard_close();
	
	return 0;
	
	//init math
	srand((unsigned)time(NULL));
	
	if ( argc == 1 )
	{
		puts("insert n (0,1,2,3,4) example, 0 accept (0,1,s,m,t,o,g)arg");
		return 0;
	}
	
	
	INT32 pin[8];
	
	switch (*argv[1])
	{
		case '0':
			if ( argc != 3 ) 
			{
				puts("insert n (0,1,2,3,4) example, 0 accept (0,1,s,m,t,o,g)arg");
				return 0;
			}
		return example_onboardled(*argv[2]);
		
		case '1':
			pin[0] = IOS_P00;
			pin[1] = IOS_P01;
			pin[2] = IOS_P02;
			pin[3] = IOS_P03;
			pin[4] = IOS_P04;
			pin[5] = IOS_P05;
			pin[6] = IOS_P06;
			pin[7] = IOS_P07;
		return example_pinout(pin);
		
		case '2':
			pin[0] = IOS_D02;
			pin[1] = IOS_D03;
			pin[2] = IOS_D04;
			pin[3] = IOS_D05;
			pin[4] = IOS_D06;
			pin[5] = IOS_D07;
			pin[6] = IOS_D08;
			pin[7] = IOS_D09;
		return example_pinout(pin);
		
		case '3': return example_328();
		case '4': return example_i328();
		
		case '5':
			pin[0] = IOS_P00;
			pin[1] = IOS_P01;
			pin[2] = IOS_P02;
			pin[3] = IOS_P03;
			pin[4] = IOS_P04;
			pin[5] = IOS_P05;
			pin[6] = IOS_P06;
			pin[7] = IOS_P07;
		return test8led(pin);
		
		case '6':
			pin[0] = IOS_P00;
			pin[1] = IOS_P01;
			pin[2] = IOS_P02;
			pin[3] = IOS_P03;
			pin[4] = IOS_P04;
			pin[5] = IOS_P05;
			pin[6] = IOS_P06;
			pin[7] = IOS_P07;
		return example_police(pin);
	}
	
	return 0;
}
*/
#endif
