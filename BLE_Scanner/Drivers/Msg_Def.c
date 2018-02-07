/*
 * Msg_Def.c
 *    Description   :
 *    Created on    : 13-May-2016 - 5:54:51 pm
 *    Author        : root
 *    Organization  : VVDN Technologies Pvt Ltd
 *
 *   The code contained herein is licensed under the GNU General Public  License. You may obtain a copy of the
 *   GNU General Public License Version 2 or later at the following locations:
 *
 *   http://www.opensource.org/licenses/gpl-license.html
 *   http://www.gnu.org/copyleft/gpl.html
 */

#include "Msg_Def.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define EN_DEBUG
#ifdef EN_DEBUG
#define DBG(fmt, args...) fprintf(stdout, "MSGDEF: Dbg %s: " fmt, __func__, ##args)
#else
#define DBG(fmt, args...)
#endif
#define ERR(fmt, args...) fprintf(stderr, "MSGDEF: Err %s: " fmt, __func__, ##args)

/* @func	: sys_mq_init
 * @param[in]	: Key for the message queue
 * @return	: message queue ID upon success; else Standard error code.
 */
int sys_mq_init(int msgKey)
{
	int qid;
	key_t key = msgKey;

	qid = msgget(key, 0);
	if (qid < 0) {
		qid = msgget(key, IPC_CREAT | 0666);
		DBG("Creat queue id:%d\n", qid);
	} else
		DBG("queue id:%d\n", qid);
	return qid;
}

/* @func	: sys_mq_kill
 * @param[in]	: Message queue Id to be removed
 * @return	: status 0
 */
int sys_mq_kill(int qid)
{
	msgctl(qid, IPC_RMID, NULL);
	DBG("Kill queue id:%d\n", qid);
	return 0;
}


int sys_mq_reset(int msgKey)
{
        int qid;
        key_t key = msgKey;

        qid = msgget(key, 0);
        if (qid < 0) {
		msgctl(qid, IPC_RMID, NULL);
	}
}

/* @func	: sys_mq_send
 * @param1[in]	: Target queue ID
 * @param2[in]	: Data buffer
 * @param3[in]	: Size of data in bytes
 * @return	: 0 if Success; else Error code.
 */
int sys_mq_send(int mqId, uint8_t *buf, uint16_t size)
{
	int status = -1;
	message_buf_t message_buff;
	message_buff.mtype = 1;
	message_buff.length = size;
	memcpy(message_buff.mtext, buf, size);
	status = msgsnd(mqId, &message_buff, MQBUFSIZE, 0);
	printf("status = %d\n",status);
	return status;
}

/* @func	: sys_mq_recv
 * @param1[in]	: Target queue ID
 * @param2[out]	: Data buffer
 * @param3[out]	: Size of data in bytes
 * @return	: 0 if Success; else Error code.
 */
int sys_mq_recv(int mqId, uint8_t *buf, uint16_t *size)
{
	int ret;
	message_buf_t message_buff;
	message_buff.mtype = 1;
	ret = msgrcv(mqId, &message_buff, MQBUFSIZE, message_buff.mtype,
			0);
	if (ret != -1) {
		memcpy(buf, message_buff.mtext, message_buff.length);
		*size = message_buff.length;
		return 0;
	} else
		printf("Messgae Que Returned Fail: %d\n", ret);
	return ret;
}

