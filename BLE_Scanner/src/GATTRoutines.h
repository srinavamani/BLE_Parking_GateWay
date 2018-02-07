/*
 * GATTRoutines.h
 *    Description   :
 *    Created on    : 29-May-2016 - 11:30:20 am
 *    Author        : root
 *    Organization  : VVDN Technologies Pvt. Ltd.
 *
 *   The code contained herein is licensed under the GNU General Public  License. You may obtain a copy of the
 *   GNU General Public License Version 2 or later at the following locations:
 *
 *   http://www.opensource.org/licenses/gpl-license.html
 *   http://www.gnu.org/copyleft/gpl.html
 */

#ifndef SRC_GATTROUTINES_H_
#define SRC_GATTROUTINES_H_

#include "GAPRoutines.h"

#define USE_128BIT_UUID	0xFFFF

#define GATT_ATTTYPE_PRIMARYSERVICE_UUID	0x2800
#define GATT_ATTTYPE_SECONDARYSERVICE_UUID	0x2801
#define GATT_ATTTYPE_INCLUDESERVICE_UUID	0x2802
#define GATT_ATTTYPE_CHARSERVICE_UUID		0x2803

#define GATT_CHARDESC_EXTPROPS_UUID	0x2900
#define GATT_CHARDESC_USERDESC_UUID	0x2901
#define GATT_CHARDESC_CCCONFIG_UUID	0x2902
#define GATT_CHARDESC_SCCONFIG_UUID	0x2903
#define GATT_CHARDESC_F0RMAT_UUID		0x2904
#define GATT_CHARDESC_AGGREGATEF0RMAT_UUID	0x2905

#define GATT_PERMISSIONS_READ	(1 << 1)
#define GATT_PERMISSIONS_WRITE	(1 << 3)
#define GATT_PERMISSIONS_NOTIFY (1 << 4)
#define GATT_PERMISSIONS_EXTPROPS (1 << 7)

#define GATT_ADDSERVICE_ENC_KEY_SIZE	0x10
#define GATT_NOTIF_NO_AUTH	0x00

#pragma pack(1)
typedef struct {
	uint16_t PrimaryServiceDeclarationHandle;
	uint16_t CharacteristicDeclarationHandle;
	uint16_t CharacteristicHandle;
	uint16_t CharacteristicExtPropsHandle;
	uint16_t CCCDescriptorHandle;
} GATT_Service_Info_t;
#pragma pack(0)

/* Forward declaration */
int GATT_AddService(uint16_t ServiceUUID);
int GATT_AddAttribute(uint16_t CharUUID);
int GATT_ExchangeMTU(uint16_t connHandle, uint8_t clientRxMTU);
int GATT_Notification(uint16_t connHandle, uint16_t charHandle, uint8_t datasize, uint8_t *packet);
int GATT_Indication(uint16_t connHandle, uint16_t charHandle, uint8_t datasize, uint8_t *packet);

#endif /* SRC_GATTROUTINES_H_ */
