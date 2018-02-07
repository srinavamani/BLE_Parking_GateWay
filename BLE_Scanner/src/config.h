/*
 * config.h
 *
 *  Created on: 11-May-2016
 *      Author: vvdnlt230
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#include "VVDNBLEMeshStack.h"

/* SmartRF06 Evaluation Board: Hardware nRF_RESET info */
/* P410 : PIN10
 * P409 : PIN15
 * P408 : PIN10
 * P411 : PIN04
 * P405 : PIN08
 * S606 : PushButton
 */

/* LH2Go Specific */
#define LH2GO_NUM_ATTRIBUTES	0x0009
#define LH2GO_CHAR_ATTR_DECL	0x0002
#define LH2GO_CHAR_ATTR_HNDL	0x0003
#define LH2GO_CHAR_ATTR_CCCD	0x0005

#define BLE_MODULE_PORT_NAME	"/dev/ttyUSB1"
#define UART_TX_PIPE	"/tmp/UARTTXPIPE"
#define EVENTPIPENAME	"/tmp/EventPipe"
#define CLOUDPIPENAME	"/tmp/CloudPipe"
#define CLOUDRESPONSEPIPENAME "/tmp/CloudResponsePipe"
#define BLE_MAC_FILE	"/tmp/ble_mac.txt"
#define BLE_INFO_FILE	"/tmp/Ble_Info.txt"	/*Do not Change it is dependent on */

#define CONN_PIPE_0		"/tmp/ConnPipe0"
#define CONN_PIPE_1		"/tmp/ConnPipe1"
#define CONN_PIPE_2		"/tmp/ConnPipe2"

#define SHOUT_PATTERN_LEN	3
#define SHOUT_DATA_BOM "BOM"
#define SHOUT_DATA_EOM "EOM"



#define DEVICE_DISCOVERY_MODE	DDM_ALL
#define DEVICE_DISCOVERY_ACTIVE_SCAN	AS_ENABLE
#define DEVICE_DISCOVERY_WHITELIST	WL_DISABLE

#define THREAD_CENTRAL_ID	0x70
#define THREAD_PERIPHERAL_ID	0x71
#define THREAD_ASYNC_ID 	0x72
#define THREAD_UART_THREAD 	0x73

#define MAX_HCI_COMMANDS	8
#define MAX_SCAN_DEVICE		10
#define MAX_CONN_DEVICE		4
#define MAX_UUID_LENGTH		16
#define MAX_UUIDS	3

#define EN_DEBUG		0

#define EVENT_CODE	0xFF

#define DEBUG_DISABLE	1
#define DIRECT_UART	1
#define MAX_CENTRAL_CONNECTIONS 1
#define CENTRAL_DELAY	10
#define PERIPHERAL_DELAY	1
#define BT_ADDRESS_LEN 6


#define QUEUE_PER_CONN_SCHEME_EN 1

#define ENABLED_RELAY_FILTER


#endif /* CONFIG_H_ */
