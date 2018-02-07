#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "Msg_Def.h"
#include "UARTDriver.h"
#include "config.h"
#include "HCI_Commands.h"
#include <semaphore.h>
#include "BLEParameters.h"

int i=0;

pthread_mutex_t NotifyMutex = PTHREAD_MUTEX_INITIALIZER;

extern UARTDriverInfo_t UARTDriverInfo;

#ifdef EN_DEBUG
#define DBG(fmt, args...) fprintf(stdout, "UARTRXTHR: Dbg %s: " fmt, __func__, ##args)
#else
#define DBG(fmt, args...)
#endif
#define ERR(fmt, args...) fprintf(stderr, "UARTRXTHR: Err %s: " fmt, __func__, ##args)

extern uint8_t SysExitFlag;

#define EventQueue 1
#define DataQueue 2

#define BLE_CMD_PKT 0x01
#define BLE_ASD_PKT 0x02
#define BLE_SYC_PKT 0x03
#define BLE_EVT_PKT 0x04

extern pthread_mutex_t CentralMutex;

typedef enum {
	STATE_IDLE = 0,
	STATE_INIT,
	STATE_EVENT_RESPONSE,
	STATE_EVENT_LENGTH,
	STATE_PACKET_WAIT,
	STATE_ERROR
} State_t;

void *UartCmdRxThread(void *args)
{
	int EventQId, DataQId, CentralQId, PeripheralQId, CommandRespQId, AsyncQId, TestRxQ, status, count, Pipe_0_Id, Pipe_1_Id, Pipe_2_Id, Pipe_3_Id,PING_PKT_RSP_ID,WRITE_RSP_ID;
	uint8_t current_state = STATE_IDLE, selectQ = 0, index = 0;
	uint8_t rx_completed, rx_byte;
	uint8_t rx_packet[2048];
	int routing_status = 0;
	uint16_t size;
	int isTerminateHandled = 0;
	int idx = 0;
	fd_set rdset;

		EventQId = sys_mq_init(EVENTQ_KEY);
		DataQId = sys_mq_init(DATAQ_KEY);
		CentralQId = sys_mq_init(CENTRAL_THREAD_QUEUE_KEY);
		PeripheralQId = sys_mq_init(PERIPHERAL_THREAD_QUEUE_KEY);
		CommandRespQId = sys_mq_init(COMMANDRESP_THREAD_QUEUE_KEY);
		AsyncQId = sys_mq_init(NOTIFRESP_THREAD_QUEUE_KEY);
		PING_PKT_RSP_ID = sys_mq_init(PING_PKT_KEY);
		WRITE_RSP_ID = sys_mq_init(WRITE_RSP_KEY);

		Pipe_0_Id = sys_mq_init(CONN_PIPE_0_KEY);

	/* Wait for BLE Firmware response */
	while (SysExitFlag == 0) {
		FD_ZERO(&rdset);
		FD_SET(UARTDriverInfo.iModemFd, &rdset);
		status = select(UARTDriverInfo.iModemFd + 1, &rdset, NULL, NULL, NULL);
		if ( status < 0) {
			printf("Select Failed\n");
		}

		/* UART RX Section we need this thread to wait on the Tx at also */
		if (FD_ISSET(UARTDriverInfo.iModemFd, &rdset)) {
			//	pthread_mutex_lock(&CommandMutex);
			read(UARTDriverInfo.iModemFd, &rx_byte, 1);
				/* printf("0x%02x ",rx_byte); Kept for debug */
				if ((index == 0) && (current_state == STATE_IDLE)
						&& ((rx_byte == BLE_EVT_PKT) || (rx_byte == BLE_ASD_PKT)))
					current_state = STATE_INIT;
				switch (current_state) {
					case STATE_IDLE:
						index = 0;
						count = 0;
						break;
					case STATE_INIT:
						if (rx_byte == BLE_EVT_PKT) {
							rx_packet[index++] = rx_byte;
							current_state = STATE_EVENT_RESPONSE;
							selectQ = EventQueue;
						} else if (rx_byte == BLE_ASD_PKT) {
							rx_packet[index++] = rx_byte;
							selectQ = DataQueue;
						} else
							current_state = STATE_ERROR;
						break;
					case STATE_EVENT_RESPONSE:
						rx_packet[index++] = rx_byte;
						current_state = STATE_EVENT_LENGTH;
						break;
					case STATE_EVENT_LENGTH:
						rx_packet[index++] = rx_byte;
						count = (int) rx_byte;
						current_state = STATE_PACKET_WAIT;
						break;
					case STATE_PACKET_WAIT:
						rx_packet[index++] = rx_byte;
						count--;
						if (count <= 0) {
							print_buffer(rx_packet,index);
//							usleep(10000);

								if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0x10) {
									printf("\n\n***********************************************************************************\n");
									printf("\nHARDWARE_FAIL_OUT_OF_MEMORY\n");
									printf("\n***********************************************************************************\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[7], rx_packet[6]) == 0xFC06) {
									printf("\n\nFC06\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[7], rx_packet[6]) == 0xFC21) {
									printf("\n\nFC21\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == 0x0604) {
									printf("\n\n0604\n\n");    
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0x0E) {
									printf("\nBLE_LED_RESPONSE\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0x0E) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									printf("\n\nBLE_MAC\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_LINK_PARAMETER_UPDATE ) {
									//sys_mq_send(CommandRespQId, rx_packet, index);
									printf("Update Param\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_READ_BY_TYPE_REQUEST ) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n0508\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_READ_BY_GROUP_TYPE_REQUEST ) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n0607\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_READ_REQUEST ) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n050A\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_EXCHANGE_MTU_REQUEST ) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n0502\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_ADVERT_DATA_UPDATE_DONE ) {
									printf("ADVERTISED\n");
									//printf("\n\n0602\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_MAKE_DISCOVERABLE_DONE ) {
									printf("Make discoverable\n");
									//printf("\n\n0603\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_COMMAND_STATUS ) {

									if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == GATT_DELSERVICE)
									{
										printf("Delete_service\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFD9B)
									{
										sys_mq_send(WRITE_RSP_ID, rx_packet, index);
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == VS_ATT_WRITE_RESPONSE)
									{
										//								printf("FD13\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFD92)
									{
										sys_mq_send(WRITE_RSP_ID, rx_packet, index);
										printf("write_ack\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFDB6)
									{
										//								sys_mq_send(WRITE_RSP_ID, rx_packet, index);
										printf("write_ack_no_rsp\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFE30){
										printf("PARAM_UPDATE SUCCESS\n");
										//printf("\n\nFE30\n\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFDFC || MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFDFE){
										printf("ATT_SERVICE DONE\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFE06 || MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFE07){
										printf("UPDATE ADV / MAKE DISCOVERABLE DONE\n");
									}
									else if(MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFE06 || MAKE16OFXY(rx_packet[7],rx_packet[6]) == 0xFE08){
										sys_mq_send(CommandRespQId, rx_packet, index);
									}
									else
									{
										sys_mq_send(CommandRespQId, rx_packet, index);
									}

									//printf("\n\n067F\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_DEVICE_INFORMATION) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n0601\n\n");                       
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_LINK_ESTABLISHED) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n0605\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_DEVICE_DISCOVERY) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n060D\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_TI_ATT_READ_BY_TYPE_RESPONSE) {
									sys_mq_send(CommandRespQId, rx_packet, index);
									//printf("\n\n0509\n\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_ATT_WRITE_RESPONSE_EVENT) {
									//                                                                sys_mq_send(CommandRespQId, rx_packet, index);
									printf("======>  0513\n");
								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && \
										MAKE16OFXY(rx_packet[4], rx_packet[3]) == VS_GAP_LINK_TERMINATED) {
									//                                                                sys_mq_send(CommandRespQId, rx_packet, index);
									uint16_t Conn_Handle = MAKE16OFXY(rx_packet[7],rx_packet[6]);
									Conn_Device_List[Conn_Handle].status=0;
									printf("Conn_Device_List[%d].status = %d\n",Conn_Handle,Conn_Device_List[Conn_Handle].status);
									if(rx_packet[8] == 0x3E)		// Peer Disconnection request
										sys_mq_send(CommandRespQId, rx_packet, index);

									printf("\nLINK_TERMINATED\n");
									Upadate_Disconnection(Conn_Handle);

								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_HANDLE_VALUE_NOTIFICATION) {

									if(rx_packet[11] == 0x30 && rx_packet[12] == 0x32)
									{
										//								sys_mq_send(CommandRespQId, rx_packet, index);
										printf("Slave_Upadate_IT's\n");
									}
									if(rx_packet[6] == 0x00)
										sys_mq_send(Pipe_0_Id, rx_packet, index);
									else if(rx_packet[6] == 0x01)
										sys_mq_send(Pipe_1_Id, rx_packet, index);
									else if(rx_packet[6] == 0x02)
										sys_mq_send(Pipe_2_Id, rx_packet, index);
									else if(rx_packet[6] == 0x03)
										sys_mq_send(Pipe_3_Id, rx_packet, index);

								} else if (rx_packet[0] == BLE_EVT_PKT && rx_packet[1] == 0xFF && (MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_WRITE_REQUEST || MAKE16OFXY(rx_packet[4], rx_packet[3]) == EVENT_VS_ATT_PREPARE_WRITE_REQUEST)) {

									if(rx_packet[13] == 0x30 && rx_packet[14] == 0x31)
									{
										sys_mq_send(CommandRespQId, rx_packet, index);
									}
									else if(rx_packet[11] == 0x05 || rx_packet[11] == 0x09)
									{
										sys_mq_send(CommandRespQId, rx_packet, index);
//										data_ack((int)(rx_packet[6]));
									}
									else if(rx_packet[13] == 0x50 && rx_packet[14] == 0x42 && rx_packet[31] == 0x50 && rx_packet[32] == 0x45)
										sys_mq_send(PING_PKT_RSP_ID, rx_packet, index);
									else if(MAKE16OFXY(rx_packet[12],rx_packet[11]) == 0x0003) // Notification
									{
										if(rx_packet[6] == 0x00)
											sys_mq_send(Pipe_0_Id, rx_packet, index);
										else if(rx_packet[6] == 0x01)
											sys_mq_send(Pipe_1_Id, rx_packet, index);
										else if(rx_packet[6] == 0x02)
											sys_mq_send(Pipe_2_Id, rx_packet, index);
										else if(rx_packet[6] == 0x03)
											sys_mq_send(Pipe_3_Id, rx_packet, index);
									}

									data_ack((int)(rx_packet[6]));
								} 
							else
							{
//								printf("\nADDITIONAL EVENT\n--------------------------------------\n\n");
							}
							selectQ = 0;
							current_state = STATE_IDLE;
							index = 0;
							count = 0;
						}
						usleep(100);
						break;
				}
			}
		}
		DBG("Exiting UartCmdRxThread...\n");
	}

void *pipe_0000_Thread(void *args)
{
	int MESSAGE_SEND_TO_PEER_DEVICE_Id = sys_mq_init(MESSAGE_SEND_TO_PEER_DEVICE_KEY);
	int idx=0,data_length,i=0;
	uint8_t buff[100],forward_data_buff[100];
	int Pipe_0_Id = sys_mq_init(CONN_PIPE_0_KEY);	
	uint16_t size;
	while(1)
	{
		
		idx = 0;
		sys_mq_recv(Pipe_0_Id, buff, &size);
		data_length=(int) buff[8];

		if(MAKE16OFXY(buff[4], buff[3]) == EVENT_VS_ATT_WRITE_REQUEST)
		{
			data_length=data_length-4;

			forward_data_buff[idx++] = 0x00;
			forward_data_buff[idx++] = 0x01;
			forward_data_buff[idx++] = 0x00 + data_length;

			for(i=13;i<data_length+13;i++)
			{
				forward_data_buff[idx++] = buff[i];
			}
		}
		else
		{
			data_length=data_length-2;

			forward_data_buff[idx++] = 0x00;
			forward_data_buff[idx++] = 0x00;
			forward_data_buff[idx++] = 0x00 + data_length;

			for(i=11;i<data_length+11;i++)
			{
				forward_data_buff[idx++] = buff[i];
			}

		}
		sys_mq_send(MESSAGE_SEND_TO_PEER_DEVICE_Id, forward_data_buff, data_length+3);
	}
}

void *Forward_MSG_Queue_Thread(void *args)
{
	sleep(8);
	int WRITE_RSP_ID = sys_mq_init(WRITE_RSP_KEY);
	int Forward_Msg_Queue_Id = sys_mq_init(MESSAGE_SEND_TO_PEER_DEVICE_KEY);
	int i=0;
	uint16_t size,size_Rx;
	uint8_t buffer[100];
	uint8_t buffer_Rx[100];
	static int ble_state = 0,resend_count = 0;
	uint8_t Conn_Handle_Id,Direction;
	int idz;

	//static uint8_t CHECK_NEXT_PKT = 0,EOM_STATUS = 0;

	uint8_t retry_count,retry_needed;

	int cloud_response_fd;
	char * myfifo_message_queue_rsp = "/tmp/myfifo_message_queue_rsp";
	mkfifo(myfifo_message_queue_rsp, 0666);

	while(1)
	{
		sys_mq_recv(Forward_Msg_Queue_Id, buffer, &size);

//		for(i=0;i<size;i++)
//			printf("0x%02X:",buffer[i]);

		Conn_Handle_Id = buffer[0];
		idz=0;
		Direction = buffer[1];
		// 0xFF is for the cloud data - Comes from cloud binary.
		if(Conn_Handle_Id == 0xFF)
		{
			if(BLE_Mode_Change.Slave_Count > 0)
			{
				// This loop will forward the message to all the BLE connections.
				for(idz = 0; idz < MAX_NUM_PERIPHERALS; idz++)
				{
					if(Conn_Device_List[idz].status == 1)
					{
						//					printf("Cloud_data_Notify_data\n");
						retry_needed = 0, retry_count = RETRY_COUNT;
						do {
							usleep(NOTIFICATION_INTERVAL);
							Notify_data(buffer, size, idz);
							if (sys_mq_recv(WRITE_RSP_ID, buffer_Rx, &size_Rx) == 0) {
								//							printf("Received any thing - %02x\n", buffer_Rx[5]);
								if (buffer_Rx[5] == 0x00) {
									retry_needed = retry_count;
									printf("SUCCESS\n");
								}
								else if (buffer_Rx[5] == 0x16){
									printf("Waiting = %d\n",retry_needed);
									if(retry_needed == 0)
										usleep(3000*2);
									else if(retry_needed == 1)
										usleep(3000*3);
									else if(retry_needed == 2)
										usleep(3000*4);
									else if(retry_needed == 3)
										usleep(3000*5);

//									retry_needed++;
								}
								else if (buffer_Rx[5] == 0x14)
								{
									printf("This Connection Disconnected\n");
//									Conn_Device_List[idz].status = 0;
//									uint16_t Conn_Handle_Disconnected = 0x0000 + idz;
//									Upadate_Disconnection(Conn_Handle_Disconnected);
									retry_needed = retry_count;
								}
								else if(buffer_Rx[5] == 0x04)
								{
									printf("MSG BUFFER NOT AVAILABLE\n");
									sleep(2);
									retry_needed = 0;
								}
							}
						} while (retry_needed < retry_count);
					}
				}
			}
		}
		else if(Conn_Handle_Id == 0xEE)  // For identification of PING packet
		{
			idz = (int)buffer[1];
			retry_needed = 0, retry_count = RETRY_COUNT;
			do {
				usleep(NOTIFICATION_INTERVAL);
				Notify_data(buffer, size, idz);
				if (sys_mq_recv(WRITE_RSP_ID, buffer_Rx, &size_Rx) == 0) {
					//							printf("Received any thing - %02x\n", buffer_Rx[5]);
					if (buffer_Rx[5] == 0x00) {
						retry_needed = retry_count;
						printf("SUCCESS\n");
					}
					else if (buffer_Rx[5] == 0x16){
						printf("Waiting = %d\n",retry_needed);
						if(retry_needed == 0)
							usleep(3000*2);
						else if(retry_needed == 1)
							usleep(3000*3);
						else if(retry_needed == 2)
							usleep(3000*4);
						else if(retry_needed == 3)
							usleep(3000*5);

						retry_needed++;
					}
					else if (buffer_Rx[5] == 0x14)
					{
						printf("This Connection Disconnected\n");
						retry_needed = retry_count;
					}
					else if(buffer_Rx[5] == 0x04)
					{
						printf("MSG BUFFER NOT AVAILABLE\n");
						sleep(2);
						retry_needed = 0;
					}
				}
			} while (retry_needed < retry_count);

		}
		else
		{
			if(Direction == 0x01)			//  0X01  ---> Slave
			{

				Notify_data(buffer, size, 0);
				sleep(1);
				GATT_WRITE_VALUE(buffer, size, 0);

				if(BLE_Mode_Change.Slave_Count > 0)
				{
					for(idz = 0; idz < MAX_NUM_PERIPHERALS; idz++)
					{
						if(Conn_Device_List[idz].status == 1 && idz != Conn_Handle_Id)
						{
							//						printf("Mobile_data_Notify_data\n");
							retry_needed = 0, retry_count = RETRY_COUNT;
							do {
								usleep(NOTIFICATION_INTERVAL);
								Notify_data(buffer, size, idz);
								//GATT_WRITE_VALUE(buffer, size, idz);
								if (sys_mq_recv(WRITE_RSP_ID, buffer_Rx, &size_Rx) == 0) {
									//                                      printf("Received any thing - %02x\n", buffer_Rx[5]);
									if (buffer_Rx[5] == 0x00) {
										retry_needed = retry_count;
										printf("SUCCESS\n");
									}
									else if (buffer_Rx[5] == 0x16){
										printf("Waiting = %d\n",retry_needed);
										if(retry_needed == 0)
											usleep(3000);
										else if(retry_needed == 1)
											usleep(3000*2);
										else if(retry_needed == 2)
											usleep(3000*3);
										else if(retry_needed == 3)
											usleep(3000*4);

										retry_needed++;
									}
									else if (buffer_Rx[5] == 0x14)
									{
										printf("This Connection Disconnected\n");
//										Conn_Device_List[idz].status = 0;
//										uint16_t Conn_Handle_Disconnected = 0x0000 + idz;
//										printf("Disconnected Conn Handle = %04x\n",Conn_Handle_Disconnected);
//										Upadate_Disconnection(Conn_Handle_Disconnected);
										retry_needed = retry_count;
									}
									else if(buffer_Rx[5] == 0x04)
									{
										printf("MSG BUFFER NOT AVAILABLE\n");
										sleep(2);
										retry_needed = 0;
									}
								}
							} while (retry_needed < retry_count);
						}
					}
				}
			}
		}

#if 0	//  This is for the Central mode write operation
		else
		{
			for(idz = 0; idz < 4; idz++)
			{
				if(Conn_Device_List[idz].status == 1 && idz != Conn_Handle_Id)
				{
					//				printf("GATT_Write_Value\n");
					//				GATT_WRITE_VALUE(buffer, size, idz);
					////
					retry_needed = 0, retry_count = 5;
					do {
						usleep(3000);
						GATT_WRITE_VALUE(buffer, size, idz);
						if (sys_mq_recv(WRITE_RSP_ID, buffer, &size) == 0) {
							if (buffer[5] == 0x14) {
								printf("================>  bleNotConnected\n");
								retry_needed = retry_count;
							}
							else if (buffer[5] == 0x00) {
								retry_needed = retry_count;
								printf("SUCCESS\n");
							}
							else
								printf("SOME_ISSUE\n");
						}
					} while (retry_needed < retry_count);
					////

				}
			}
			usleep(500);
		}
#endif		
	}
}

int data_ack(int idz)
{
	uint8_t pac[10] = {};
	int idx=0,status1=-1;
	uint16_t dat_ack_Conn_Handle = 0x0000 + idz;

	pac[idx++] = 0x01;
	pac[idx++] = 0x13;
	pac[idx++] = 0xFD;
	pac[idx++] = 0x02;
	pac[idx++] = LSBYTEOF16BIT(dat_ack_Conn_Handle);
	pac[idx++] = MSBYTEOF16BIT(dat_ack_Conn_Handle);

	if (BLECOMWrite(pac, idx) == idx){
		status1 = 0;
	}
	else {
		status1 = -1;
	}
}

// 01 92 FD 06 FE FF 34 12 AA AA

// FD92 - OPCODE , 06 - Length , FFFE - Conn_Handle , 1234 - Handle_ID , AAAA - DATA

int GATT_WRITE_VALUE(uint8_t *buffer, int size, int idz)		// HandleID + 1 for send (master write in peripheral cuuid)
{
//printf("GATT_Write\n");
        uint8_t pac[100] = {};
        int idx=0,status=-1;

        pac[idx++] = PKT_COMMAND;
#if 1//WRITE_WITHOUT_RSP
        pac[idx++] = LSBYTEOF16BIT(GATT_WRITECHARVALUE);
        pac[idx++] = MSBYTEOF16BIT(GATT_WRITECHARVALUE);
#else
        pac[idx++] = LSBYTEOF16BIT(GATT_WRITENORSP);
        pac[idx++] = MSBYTEOF16BIT(GATT_WRITENORSP);
#endif
        pac[idx++] = (0x00 + (size - 3) + 4);  // 0x00 + (size + 3byte header) = size-3 => Data + 4(CONN_HANDLE+HANDLE_ID)
        pac[idx++] = 0x00 + idz;
        pac[idx++] = 0x00;
//        pac[idx++] = LSBYTEOF16BIT(Conn_Device_List[idz].Handle_id + 1);
//        pac[idx++] = MSBYTEOF16BIT(Conn_Device_List[idz].Handle_id + 1);
        pac[idx++] = 0x03;
        pac[idx++] = 0x00;

		// OFFSET
//		pac[idx++] = 0x00;
//		pac[idx++] = 0x00;

for(i=3;i<size;i++)
	pac[idx++] = buffer[i];

//for(i=0;i<idx;i++)
//	printf("%02x_",pac[i]);

        if (BLECOMWrite(pac, idx) == idx)
                status = 0;
        else {
                DBG("BLECOMWrite failed\n");
                status = -1;
        }
return status;
}

int Notify_data(uint8_t *buffer, int size, int idz)		// HandleID + 1 for send (slave notify/update(write) its own cuuid)
{
        uint8_t pac[100] = {};
        int idx=0,status=-1;

        pac[idx++] = PKT_COMMAND;
        pac[idx++] = LSBYTEOF16BIT(GATT_NOTIFICATION);
        pac[idx++] = MSBYTEOF16BIT(GATT_NOTIFICATION);
        pac[idx++] = (0x00 + (size - 3) + 5);  // 0x00 + (size + 3byte header) = size-3 => Data + 4(CONN_HANDLE+HANDLE_ID)
//        pac[idx++] = LSBYTEOF16BIT(Conn_Device_List[idz].Conn_Handle);
//        pac[idx++] = MSBYTEOF16BIT(Conn_Device_List[idz].Conn_Handle);
pac[idx++] = 0x00;
pac[idx++] = 0x00;
		pac[idx++] = 0x00;
pac[idx++] = 0x07;
pac[idx++] = 0x00;
//        pac[idx++] = LSBYTEOF16BIT(Conn_Device_List[idz].Handle_id + 5/*1*/);	// 0x0002 + 5 write Slave to Master
//        pac[idx++] = MSBYTEOF16BIT(Conn_Device_List[idz].Handle_id + 5/*1*/);

for(i=3;i<size;i++)
{
	pac[idx++] = buffer[i];
}

//for(i=0;i<size;i++)
//	printf("%02X :",pac[i]);

        if (BLECOMWrite(pac, idx) == idx)
		{
                status = 0;
		}
        else {
                DBG("BLECOMWrite failed\n");
                status = -1;
        }

return status;
}

void Upadate_Disconnection(uint16_t Conn_Handle)
{
	int i=0, Device_Conn_Count=0;
	for(i=0;i<MAX_NUM_PERIPHERALS;i++)
	{
		if(Conn_Device_List[i].status == 1)
		{
			char Buff[100];
			Device_Conn_Count+=1;
			printf("Conn_Device_List[%d].status = %d\n",i,Conn_Device_List[i].status);
			sprintf(Buff, "Conn_Device_List[%d].status = %d\n",i,Conn_Device_List[i].status);
		}
	}
	printf("Device_Conn_Count = %d",Device_Conn_Count);
	if(Device_Conn_Count == 0)
	{
		char Buff[64];
		BLE_Mode_Change.Slave_Count = 0;
		printf("Slave_Count_0 = %d\n",BLE_Mode_Change.Slave_Count);
		sprintf(Buff, "Slave_Count_0 = %d\n",BLE_Mode_Change.Slave_Count);
	}
	BLE_Mode_Change.Slave_Count = Device_Conn_Count;
}

