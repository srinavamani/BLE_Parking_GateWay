/*
 * UARTDriver.c
 *
 *  Created on: 10-May-2016
 *      Author: vvdnlt230
 */

#include <pthread.h>
#include "UARTDriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errorcommands.h>
#include <unistd.h>
#include <fcntl.h>

extern err_db_t err_db[5];
UARTDriverInfo_t UARTDriverInfo;
pthread_mutex_t CommandMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Driver Open For the UART
 */
int BLECOMOpen(char *dev_name)
{
	printf("%s\n",dev_name);
	printf("%s %s\n", __func__, dev_name);
	/* Validation of the Device Name If Needed */
	UARTDriverInfo.iModemFd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (UARTDriverInfo.iModemFd < 0) {
		printf("%s\n", err_db[ERR_DEVICE_OPEN_FAILED].errname);
		return -1;
	}
/*
	UARTDriverInfo.iModemWdFd = open(dev_name, O_WRONLY | O_NOCTTY | O_NDELAY);
	if (UARTDriverInfo.iModemWdFd < 0) {
		printf("%s\n", err_db[ERR_DEVICE_OPEN_FAILED].errname);
		return -1;
	}
*/
	fcntl(UARTDriverInfo.iModemFd, F_SETFL, O_RDWR);
	/* fcntl(UARTDriverInfo.iModemWdFd, F_SETFL, O_WRONLY); */

	/* get the current options */
	tcgetattr(UARTDriverInfo.iModemFd, &UARTDriverInfo.options);
	/* set raw input, 1 second timeout */
	UARTDriverInfo.options.c_cflag |= (CLOCAL | CREAD);
	UARTDriverInfo.options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	UARTDriverInfo.options.c_oflag &= ~OPOST;
	cfmakeraw(&UARTDriverInfo.options);
	UARTDriverInfo.options.c_cc[VMIN] = 1;
	UARTDriverInfo.options.c_cc[VTIME] = 0;
	UARTDriverInfo.baudrate = B115200;
	cfsetospeed(&UARTDriverInfo.options, UARTDriverInfo.baudrate);
	cfsetispeed(&UARTDriverInfo.options, UARTDriverInfo.baudrate);
	/*
	options.c_ispeed=115200;
	options.c_ospeed=115200;
	tcsetattr(UARTDriverInfo.iModemRdFd, TCSANOW, &UARTDriverInfo.options);
	tcsetattr(UARTDriverInfo.iModemWdFd, TCSANOW, &UARTDriverInfo.options);
	*/
	tcsetattr(UARTDriverInfo.iModemFd, TCSAFLUSH, &UARTDriverInfo.options);
	/* tcsetattr(UARTDriverInfo.iModemWdFd, TCSAFLUSH, &UARTDriverInfo.options); */
	sleep(2); /* required to make flush work, for some reason */
	tcflush(UARTDriverInfo.iModemFd, TCIOFLUSH);
	/* tcflush(UARTDriverInfo.iModemWdFd, TCIOFLUSH); */
	return UARTDriverInfo.iModemFd; /*Returning The File Descriptor*/
}

/*
 * Driver Close For the UART
 * */
int BLECOMClose()
{
	/* device_fops.device_close(UARTDriverInfo.UARTFileDesc); */
	close(UARTDriverInfo.iModemFd);
	/* close(UARTDriverInfo.iModemWdFd); */
	return 0;
}

/*
 * Driver Write For the UART
 * */
int BLECOMWrite(unsigned char *wBytes, int iLen)
{
	ssize_t iWritten;
	int idx;
	/* pthread_mutex_lock(&CommandMutex);	//Lock the Mutex */
	iWritten = write(UARTDriverInfo.iModemFd, wBytes, iLen);
	/* pthread_mutex_unlock(&CommandMutex);	//Unlock the Mutex */
#if 1 /* Kept for debug */
	printf("\nData sent  -  ");
	for (idx = 0; idx < iLen; idx++)
		printf("%x ", wBytes[idx]);
	printf("\n ");
#endif

	return iWritten; /* Return Value Will be the Number of Bytes Written */
}

/*
 * Driver Read For the UART
 * */
int BLECOMRead(char *rBytes, int iLen)
{
	ssize_t iRead;
	int idx1;
	iRead = read(UARTDriverInfo.iModemFd, rBytes, iLen);

#if 0 /* Kept for debug */
printf("\nData received  -  ");
        for (idx1 = 0; idx1 < iLen; idx1++)
                printf("%x ", rBytes[idx1]);
        printf("\n ");
#endif

	if (iRead < 0) {
		printf("%s\n", err_db[ERR_DEVICE_READ_FAILED].errname);
		return -1;
	} else if (iRead > 0) {
		/* printf("%c,%d\n", rBytes, iRead); // Kept for debug */
		usleep(10000);
	}

	return iRead;	/* Return Value Will be the number of Bytes Read */
}

