/*
 * errorcommands.c
 *
 *  Created on: 11-May-2016
 *      Author: vvdnlt230
 */

#include "errorcommands.h"

const err_db_t err_db[] = {
	{.errcode = ERR_DEVICE_OPEN_FAILED, .errname = "[Open Error] : Device Opening Failed\n"},
	{ERR_DEVICE_WRITE_FAILED, "[Write Error] : Device Writing Failed\n"},
	{ERR_DEVICE_READ_FAILED, "[Read Error] : Device Reading Failed\n"},
	{ERR_COMMAND_RESPONSE_FAILED, "[Response Error] : Inavlid Stack Response\n"},
	{ERR_GAP_PROFILE_NOT_SUPPORTED, "[GAP Error]: Profile Not Supported\n"},
	{ERR_LINK_ESTABLISHED, "[LINK Establish]: Failed To Establish The Link\n"},
};

