/*
 * UARTDriver.h
 *
 *  Created on: 10-May-2016
 *      Author: vvdnlt230
 */

#ifndef UARTDRIVER_H_
#define UARTDRIVER_H_

#include <pthread.h>
#include <termios.h>

/*Semaphore Implementation*/
typedef struct {
	int iModemFd;
	/* int iModemWdFd; */
	struct termios options;
	speed_t baudrate ;
	pthread_mutex_t UARTLock;
} UARTDriverInfo_t;

/*Device Pointers Mapping
 * For the UART Read Write*/
typedef struct {
	int (*device_open)(char *);
	int (*device_close)(int);
	int (*device_read)(char *);
	int (*device_write)(char *);
	int (*device_ctl)(void *);
} device_fops_t;

#endif /* UARTDRIVER_H_ */
