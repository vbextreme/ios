//need app is called with super user
int example_onboardled(char mode)
{	
	HIOS u = ios_open("ios/u3l","w");
		if ( !u ) {puts("Not open");return -1;}
	
	switch (mode)
	{
		case '0':
			if ( !ios_write(u,&IOS_PIN_LOW,1)) {puts("err write");ios_close(u);return -1;}
		break;
		case '1':
			if ( !ios_write(u,&IOS_PIN_HIGH,1)) {puts("err write");ios_close(u);return -1;}
		break;
		case 's':
			if ( ios_ioctl(u,IOS_IOCTL_U3L_SD,NULL) ) {puts("err ioctl");ios_close(u);return -1;}
		break;
		case 'm':
			if ( ios_ioctl(u,IOS_IOCTL_U3L_MMC,NULL) ) {puts("err ioctl");ios_close(u);return -1;}
		break;
		case 't':
			if ( ios_ioctl(u,IOS_IOCTL_U3L_TIMER,NULL) ) {puts("err ioctl");ios_close(u);return -1;}
			int v = 300;
			if ( ios_ioctl(u,IOS_IOCTL_U3L_TIMER_ON,&v) ) {puts("err set on ioctl");ios_close(u);return -1;}
			if ( ios_ioctl(u,IOS_IOCTL_U3L_TIMER_OFF,&v) ) {puts("err set off ioctl");ios_close(u);return -1;}
		break;
		case 'o':
			if ( ios_ioctl(u,IOS_IOCTL_U3L_ONESHOT,NULL) ) {puts("err ioctl");ios_close(u);return -1;}
		break;
		case 'g':
			if ( ios_ioctl(u,IOS_IOCTL_U3L_GPIO,NULL) ) {puts("err ioctl");ios_close(u);return -1;}
		break;
		default:
			puts("err argument");
		return -1;
	}
	
	ios_close(u);
	
	return 0;
}