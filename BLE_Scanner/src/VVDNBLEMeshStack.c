#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "config.h"
#include "GPIOAccess.h"
#include "BLEParameters.h"

uint8_t SysExitFlag = 0;
uint8_t ThreadStatus;

pthread_t tUartCmdRxThread;
pthread_t tCentralModeThread;
pthread_t tPeripheralCentralModeThread;
pthread_t tCentralPeripheralModeThread_Watcher;

pthread_t tForward_MSG_Queue_Thread;
pthread_t tMSG_Queue_Thread;
pthread_t tDynamicLinkMonitorThread;
pthread_t tPing_Pkt_MonitorThread;
pthread_t tCloud_Data_Thread;
extern void *UartCmdRxThread(void *args);
extern void *CentralModeThread(void *args);
extern void *PeripheralCentralModeThread(void *args);
extern void *Cloud_Data_Thread(void *args);
extern void *CentralModeThread(void *args);

extern void *Forward_MSG_Queue_Thread(void *args);
extern void *MSG_Queue_Thread(void *args);
extern void *Dynamic_Link_Monitor_Thread(void *args);
extern void *Ping_Pkt_Monitor_Thread(void *args);

#define BLEttyPath "/dev/ttyACM0"

int main(void)
{

if(BLECOMOpen(BLEttyPath) < 0)
{
	printf("Port Opening Error - %s\n",BLEttyPath);
	return 0;
}

	if (pthread_create(&tUartCmdRxThread, NULL, UartCmdRxThread, NULL)) {
			printf("UartCmdRxThread creation failed\n");
			SysExitFlag = 1;
	}

	if (pthread_create(&tCentralModeThread, NULL, CentralModeThread, NULL)) {
		printf("CentralModeThread creation failed\n");
		SysExitFlag = 1;
	}

	pthread_join(tUartCmdRxThread, NULL);

	sleep(1);
	printf("Exiting Application\n");
	return 0;
}
