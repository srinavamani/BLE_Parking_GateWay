/*
 * BLEParameters.c
 *
 *  Created on: 06-Jun-2016
 *      Author: root
 */

#include <stdint.h>
#include "HCI_Commands.h"
#include "BLEParameters.h"

typedef struct {
	BLEParameters_t BLEparameters;
	uint8_t ParameterCode;
} BLEDatabase_t;

#pragma pack(1)
typedef struct {
	uint8_t type;
	uint16_t opcode;
	uint8_t DataLength;
	uint8_t ParamId;
	uint16_t ParamValue;
} SetParamValue_t;

typedef struct {
	uint8_t type;
	uint8_t EventCode;
	uint8_t DataLength;
	uint16_t Event;
	uint8_t Status;
	uint16_t opcode;
	uint8_t Length;
} SetParamValueResp_t;

#pragma pack(0)
#if 0
-Type           : 0x04 (Event)
-EventCode      : 0x00FF (Event)
-Data Length    : 0x06 (6) bytes(s)
 Event          : 0x067F (1663) (GAP_HCI_ExtentionCommandStatus)
 Status         : 0x00 (0) (Success)
 OpCode         : 0xFE30 (GAP_SetParam)
 DataLength     : 0x00 (0)
#endif

int SetParamValue(uint8_t ParamId, uint16_t ParamValue, int QueueId)
{
	uint8_t buffer[10];
	SetParamValue_t *SetParamValue;
	int size;
	SetParamValue = (SetParamValue_t *)buffer;
	SetParamValueResp_t *SetParamValueResp;

	SetParamValue->type = PKT_COMMAND;
	SetParamValue->opcode = VS_GAP_SET_PARAMETER;
	SetParamValue->DataLength = 0x03;

	switch (ParamId) {
	case TGAP_GEN_DISC_ADV_MIN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_LIM_ADV_TIMEOUT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_GEN_DISC_SCAN:
		/* Scan Timeout Need To Verify */
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_LIM_DISC_SCAN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_ADV_TIMEOUT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_PARAM_TIMEOUT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_LIM_DISC_ADV_INT_MIN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_LIM_DISC_ADV_INT_MAX:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_GEN_DISC_ADV_INT_MIN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_GEN_DISC_ADV_INT_MAX:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_ADV_INT_MIN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_ADV_INT_MAX:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_SCAN_INT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_SCAN_WIND:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_HIGH_SCAN_INT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_HIGH_SCAN_WIND:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_GEN_DISC_SCAN_INT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_GEN_DISC_SCAN_WIND:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_LIM_DISC_SCAN_INT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_LIM_DISC_SCAN_WIND:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_ADV:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_INT_MIN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_INT_MAX:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_SCAN_INT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_SCAN_WIND:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_MAX_CE_LEN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_PRIVATE_ADDR_INT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_SM_TIMEOUT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_SM_MIN_KEY_LEN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_SM_MAX_KEY_LEN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_FILTER_ADV_REPORTS:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_SCAN_RSP_RSSI_MIN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case SET_RX_DEBUG:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_MIN_CE_LEN:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_LATENCY:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	case TGAP_CONN_EST_SUPERV_TIMEOUT:
		SetParamValue->ParamId = ParamId;
		SetParamValue->ParamValue = ParamValue;
		break;
	}
	BLECOMWrite(buffer, sizeof(SetParamValue_t));
	if (sys_mq_recv(QueueId, buffer, &size) == 0) {
		print_buffer(buffer, size);
		SetParamValueResp = (SetParamValueResp_t *)buffer;
		if (SetParamValueResp->Status == 0x00 && SetParamValueResp->Event == VS_COMMAND_STATUS 
				&& SetParamValueResp->opcode == VS_GAP_SET_PARAMETER && SetParamValueResp->EventCode == 0xFF 
				&& SetParamValueResp->type == PKT_EVENT ) {
			/* Setting Parameter Success */
			return 0;
		} else {
			return -1;
		}
	}
	return -1;
}

#if 0
TGAP_CONN_EST_SUPERV_TIMEOUT = 20 Seconds;
TGAP_CONN_EST_LATENCY = 0;
TGAP_CONN_EST_SCAN_WIND = 80;
TGAP_CONN_EST_SCAN_INT = 80;
TGAP_GEN_DISC_SCAN_WIND = 80;
TGAP_GEN_DISC_SCAN = 0;
#endif

