/*
 * errorcommands.h
 *
 *  Created on: 11-May-2016
 *      Author: vvdnlt230
 */

#ifndef ERRORCOMMANDS_H_
#define ERRORCOMMANDS_H_

typedef enum {
	ERR_DEVICE_OPEN_FAILED,
	ERR_DEVICE_WRITE_FAILED,
	ERR_DEVICE_READ_FAILED,
	ERR_COMMAND_RESPONSE_FAILED,
	ERR_GAP_PROFILE_NOT_SUPPORTED,
	ERR_LINK_ESTABLISHED,
} errcodes_t;

typedef struct {
	errcodes_t errcode;
	char *errname;
} err_db_t;

#endif /* ERRORCOMMANDS_H_ */
