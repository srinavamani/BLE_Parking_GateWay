/*
 * GPIOAccess.c
 *    Description   : HardReset GPIO routines.
 *    Created on    : Wednesday, 29 June 2016, 18:49:46 IST
 *    Author        : root
 *    Organization  : VVDN Technologies Pvt. Ltd.
 *
 *   The code contained herein is licensed under the GNU General Public  License. You may obtain a copy of the
 *   GNU General Public License Version 2 or later at the following locations:
 *
 *   http://www.opensource.org/licenses/gpl-license.html
 *   http://www.gnu.org/copyleft/gpl.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "GPIOAccess.h"

/* @func : gpio_io_init
 *
 * Initializes the GPIOs. also inits
 * the specified GPIO pin with the direction
 * each GPIO should be initialized once before using. trying to init
 * initializing all the GPIOs at once. hence applications need not init GPIOs
 */
void gpio_io_init(int pin, char *direction)
{
	char export_dir[50], gpio_dir[50], fname[50];
	int pin_no = pin;
	char dir[5];

	strcpy(dir, direction);
	sprintf(export_dir, "echo %d > /sys/class/gpio/export", pin_no);
	sprintf(fname, "/sys/class/gpio/gpio%d", pin_no);
	if (access(fname, F_OK) == -1)
		system(export_dir);
	sprintf(gpio_dir, "echo %s > /sys/class/gpio/gpio%d/direction", dir, pin_no);
	system(gpio_dir);
}

/* @func : gpio_io_read
 * This function reads the GPIO pin specified. the read can be of 2 types.
 * @arg1 : int pin; The GPIO pin number to be read
 */
char gpio_io_read(int pin_no)
{
	int gpio_fd = 0, ret;
	char val = '0', Dev_path[50];

	sprintf(Dev_path, "/sys/class/gpio/gpio%d/value", pin_no);
	gpio_fd  = open(Dev_path, O_RDWR);
	ret = read(gpio_fd, &val, 1);
	if (ret == 0)
		fprintf(stderr, "%s read error\n", Dev_path);
	close(gpio_fd);
	return val;
}

/* @func : gpio_io_write
 *
 * Function to write the values to the  GPIO OUTPUT pins. This function will
 * write the single bit value pointed by the pointer parameter specified.
 * The GPIO pin supplied must be of type OUTPUT.
 *
 * @arg1 : int pin; The GPIO pin to which write is to be done.
 * @arg2 : char val; The Value to be written
 */
void gpio_io_write(int pin_no, char *val)
{
	int gpio_fd = 0;
	char Dev_path[50];

	sprintf(Dev_path, "/sys/class/gpio/gpio%d/value", pin_no);
	gpio_fd  = open(Dev_path, O_RDWR);
	write(gpio_fd, val, 1);
	close(gpio_fd);
}


/* @func : BLE_HW_Reset_Init
 *
 * Initialize and configure the HW Reset GPIO for BLE module of Argon Board.
 * @param[in]	: GPIO number.
 */
void BLE_HW_Reset_Init(int gpio)
{
	gpio_io_init(gpio, BLE_RESET_GPIO_DIR_OUT);
	BLE_HW_Reset(gpio);
}

/* @func : BLE_HW_Reset
 *
 * Function will assert a Logic LOW at BLE_HW_RESET_GPIO of Argon Board.
 * @param[in]	: GPIO number.
 */
void BLE_HW_Reset(int gpio)
{
	int ret;

	gpio_io_write(gpio, "0");
	sleep(2);
	gpio_io_write(gpio, "1");
	sleep(1);
}
