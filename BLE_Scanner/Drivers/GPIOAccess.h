/*
 * GPIOAccess.h
 *    Description   : HardReset GPIO routine header file.
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

/* BLE Hardware Reset Scheme */
/* Require : IO Expander */

#define BLE_RESET_GPIO_J1_40	40
#define BLE_RESET_GPIO_J1_39	125

#define BLE_RESET_GPIO_DIR_IN	"in"
#define BLE_RESET_GPIO_DIR_OUT	"out"

/* Forward Declaration */
void gpio_io_init(int pin, char *direction);
char gpio_io_read(int pin_no);
void gpio_io_write(int pin_no, char *val);
void BLE_HW_Reset_Init(int gpio);
void BLE_HW_Reset(int gpio);

