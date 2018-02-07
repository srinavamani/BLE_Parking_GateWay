/*
 * Msg_Def.h
 *    Description   :
 *    Created on    : 13-May-2016 - 5:46:28 pm
 *    Author        : root
 *    Organization  : VVDN Technologies Pvt Ltd
 *
 *   The code contained herein is licensed under the GNU General Public  License. You may obtain a copy of the
 *   GNU General Public License Version 2 or later at the following locations:
 *
 *   http://www.opensource.org/licenses/gpl-license.html
 *   http://www.gnu.org/copyleft/gpl.html
 */

#ifndef DRIVERS_MSG_DEF_H_
#define DRIVERS_MSG_DEF_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>

#define EVENTQ_KEY											1990
#define DATAQ_KEY											1991
//#define pipe_0_queue_ID									1992
#define CENTRAL_THREAD_QUEUE_KEY							1993
#define PERIPHERAL_THREAD_QUEUE_KEY							1994
#define COMMANDRESP_THREAD_QUEUE_KEY						1995
#define TESTRXQUEUEKEY										1996
#define NOTIFRESP_THREAD_QUEUE_KEY							1997
#define WRITE_RSP_KEY										1998
#define PING_PKT_KEY										1999
#define CONN_PIPE_0_KEY										2000
#define CONN_PIPE_1_KEY										2001
#define CONN_PIPE_2_KEY										2002
#define CONN_PIPE_3_KEY										2003
#define MESSAGE_SEND_TO_PEER_DEVICE_KEY						2004
#define CONNHANDLE_QUEUE_ID									2005

#define MQBUFSIZE (sizeof(message_buf_t)-sizeof(long))

typedef struct buf {
	long mtype;
	uint16_t length;
	uint8_t mtext[35];
} message_buf_t;

/* Forward declaration */
int mq_init(int msgKey);
int mq_kill(int qid);
int sys_mq_send(int mqId, uint8_t *buf, uint16_t size);
int sys_mq_recv(int mqId, uint8_t *buf, uint16_t *size);

#endif /* DRIVERS_MSG_DEF_H_ */
