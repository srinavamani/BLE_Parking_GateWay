#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "Msg_Def.h"
#include "config.h"
#include "HCI_Commands.h"
#include "errorcommands.h"
#include <string.h>
#include "BLEParameters.h"

#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <semaphore.h>

#define INTERVAL_LIMIT	6	
#define MAXSIZE     27
#define ON	1
#define OFF	0
#define NORMAL	0
#define FORCE	1
#define HARD_CODED_SERVICE_ID 1
#define NOTIFICATION_HANDLE_COUNT 2

#define GPIOFILENAME "/sys/class/gpio/gpio121/value"

#define TIMING_FOR_CENTRAL_MODE		5
#define SUUID_SIZE			2  // In BYTES
#define CUUID_SIZE			16  // In BYTES
#define SIZE_OF_MASTER_ID		6

#define NETWORK_ID_SIZE			6
#define MASTER_ID_SIZE			6
#define MY_ID				6
#define TIME_STAMP_SIZE			8
#define MY_ID_FILE_PATH	"/tmp/MY_ID"

pthread_t tCentral_Mode_Thread;
pthread_t tPeripheral_Mode_Thread;
pthread_t tUser_ID_Thread;

extern void *Central_Mode_Thread(void *args);
extern void *Peripheral_Mode_Thread(void *args);
extern void *User_ID_Thread(void *args);

static uint8_t Network_ID[NETWORK_ID_SIZE+1] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x31};
static uint8_t Master_ID_1[MASTER_ID_SIZE+1];
static uint8_t Master_ID_2[MASTER_ID_SIZE+1];
static uint8_t timestamp[15];
static uint8_t adv_pkt_data_Free[2] = {0x46};
static uint8_t adv_pkt_data_Slave[2] = {0x53};
static uint8_t My_ID[MY_ID+1];
static uint8_t MY_MAC[20] = {0};

static int Rough_Slave_Count;
static int tCommandRespQId,handle_count=0,NOOFHANDLES=1,WRITE_RSP_ID;
uint8_t buffer[256],AddrType[1];
uint16_t size;
char Scanned_MAC_Buff[7] = {0};
uint16_t Conn_Handle,Handle_ID,Handle_ID_Main,Handle_ID1,Handle_ID_Main1;

uint8_t	DELETE_SERVICE_FLAG = 1;

char *BB_DETAILS_FILE = "/tmp/chatter_box_encrypt";
char data[512];
char temp[512];

static int shmid;
key_t key;
char *shm, *s;
static int shared_memory_status = 0;
key = 5678;

//extern uint8_t SysExitFlag;
void print_buffer(unsigned char *buffer, int size);

typedef enum {
	DYNAMIC_LINK_PING_NONE,
	DYNAMIC_LINK_PING_SENT,
	DYNAMIC_LINK_PING_RECEIVED,
	DYNAMIC_LINK_PING_TIMEOUT,
	DYNAMIC_LINK_PING_DISCONNECT_SENT,
} Ping_State_t;

typedef enum {
	DYNAMIC_LINK_TIMER_OFF,
	DYNAMIC_LINK_TIMER_RUNNING,
	DYNAMIC_LINK_TIMER_EXPIRED,
} Ping_Timer_Status_t;

struct itimerval tout_val;
int howmany = 0;
static int timer_status;
static int timer_status_2;
static int ping_status;


typedef enum {
	STATE_CENTRAL_INIT,
	STATE_CENTRAL_DEVICE_SCAN_REQ,
	STATE_CENTRAL_DEVICE_SCAN_RSP,
	STATE_CENTRAL_DEVICE_SCAN_DATA,
	STATE_CENTRAL_DEVICE_SCAN_CANCEL,
	STATE_CENTRAL_DEVICE_SCAN_CANCEL_RSP,
	STATE_CENTRAL_DEVICE_ESTABLISH_LINK_REQ,
	STATE_CENTRAL_DEVICE_ESTABLISH_LINK_RSP,
	STATE_CENTRAL_DEVICE_DISCOVER_CHAR_USING_UUID,
	STATE_CENTRAL_DEVICE_DISCOVER_CHAR_USING_UUID_RSP,
	STATE_CENTRAL_ENABLE_NOTIFICATION,
	STATE_CENTRAL_ENABLE_NOTIFICATION_RSP,
	STATE_CENTRAL_DEVICE_WRITE_MASTER_ID,
	STATE_CENTRAL_DEVICE_WRITE_MASTER_ID_RSP,
	STATE_CENTRAL_UPDATE_CONN_DEVICE,
} CentralState_new_t;

void Notify_Ack()
{
	uint8_t pac[10] = {};
	int idx=0,status1=-1;
	pac[idx++] = 0x01;
	pac[idx++] = 0x13;
	pac[idx++] = 0xFD;
	pac[idx++] = 0x02;
	pac[idx++] = LSBYTEOF16BIT(Conn_Handle);
	pac[idx++] = MSBYTEOF16BIT(Conn_Handle);

	if (BLECOMWrite(pac, idx) == idx){
		status1 = 0;
	}
	else {
		status1 = -1;
	}
	return status1;
}

int HCI_Reset()
{
	uint8_t pac[10] = {};
	int idx=0,status=-1;
	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(BT_RESET);	//	0x03 Opcode LSB 
	pac[idx++] = MSBYTEOF16BIT(BT_RESET);	//	0x0C Opcode MSB 
	pac[idx++] = 0x00;						//	0x00 Data Length

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
}

/*
   [4] : <Tx> - 02:57:38.487
   -Type           : 0x01 (Command)
   -OpCode         : 0xFE00 (GAP_DeviceInit)
   -Data Length    : 0x26 (38) byte(s)
   -ProfileRole    : 0x08 (8) (Central)
   -MaxScanRsps    : 0x14 (20)
   -IRK            : 00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00
   -CSRK           : 00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00
   -SignCounter    : 0x00000001 (1)
   -Dump(Tx):
0000:01 00 FE 26 08 14 00 00 00 00 00 00 00 00 00 00 ...&............
0010:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
0020:00 00 00 00 00 00 01 00 00 00                   ..........
*/

int HCI_Init()
{
	uint8_t pac[50] = {};
	int i = 0, idx = 0, status = -1;
	pac[idx++] = PKT_COMMAND;				
	pac[idx++] = LSBYTEOF16BIT(VS_GAP_DEVICE_INITIALIZATION);   //  0x00 Opcode LSB 
	pac[idx++] = MSBYTEOF16BIT(VS_GAP_DEVICE_INITIALIZATION);   //  0xFE Opcode MSB 
	pac[idx++] = 0x26;
	pac[idx++] = 0x08;	// Central Mode Only
	pac[idx++] = 0x14;	// Max Scan Rsp
	for(i = 0; i < 16; i++)	// IRK
		pac[idx++] = 0x00;
	for(i = 0; i < 16; i++)	// CSRK
		pac[idx++] = 0x00;
	for(i = 0; i < 3; i++)	// SignCounter
		pac[idx++] = 0x00;
	pac[idx++] = 0x01;

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}

}

int Device_Establish_Link_Req()
{
	uint8_t pac[10] = {}; 
	int idx=0,status=-1,i=5;
	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(VS_GAP_ESTABLISH_LINK_REQUEST); /* 0x09 Opcode LSB */
	pac[idx++] = MSBYTEOF16BIT(VS_GAP_ESTABLISH_LINK_REQUEST); /* 0xFE Opcode MSB */
	pac[idx++] = 0x09;
	pac[idx++] = 0x00;
	pac[idx++] = 0x00;
	pac[idx++] = 0x01;

	pac[idx++] = Scanned_MAC_Buff[5];
	pac[idx++] = Scanned_MAC_Buff[4];
	pac[idx++] = Scanned_MAC_Buff[3];
	pac[idx++] = Scanned_MAC_Buff[2];
	pac[idx++] = Scanned_MAC_Buff[1];
	pac[idx++] = Scanned_MAC_Buff[0];

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
	return status;
}

int Device_Establish_Link_Rsp()
{
	int status=-1,i=0,j=0;

	sys_mq_recv(tCommandRespQId, buffer, &size);

	if(buffer[0] == 0x04 && buffer[1] == 0xFF)
	{
		printf("\n");
		if(MAKE16OFXY(buffer[4],buffer[3]) == VS_GAP_LINK_ESTABLISHED)
		{
			printf("\n");
			printf("BLE_MAC = ");
			for(i=0,j=12;i<6;i++,j--)
			{			
				Scanned_MAC_Buff[i]=buffer[j];
				printf("%02X:",(unsigned int)Scanned_MAC_Buff[i]);
			}
			printf("\n");

			Conn_Handle = MAKE16OFXY(buffer[14],buffer[13]);
			printf("%04X\n",Conn_Handle);
			return 0;
		}
		else if(MAKE16OFXY(buffer[4],buffer[3]) == 0x0606)
		{
			if(buffer[8] == 0x3E)
			{
				return 2;
			}
		}
		else if(MAKE16OFXY(buffer[7],buffer[6]) == 0xFE08)   // VS_GAP_END_DISCOVERABLE
		{
			return 3;
		}
	}
	else
		return 5;	// BLE_MAC fuction to avoid struck

}

int Enable_Notification()
{
	uint8_t pac[10] = {}; 
	int idx=0,status=-1;

	pac[idx++] = PKT_COMMAND;
	//#if WRITE_WITHOUT_RSP
	pac[idx++] = LSBYTEOF16BIT(GATT_WRITECHARVALUE);
	pac[idx++] = MSBYTEOF16BIT(GATT_WRITECHARVALUE);
	//#else
	//        pac[idx++] = LSBYTEOF16BIT(GATT_WRITENORSP);
	//        pac[idx++] = MSBYTEOF16BIT(GATT_WRITENORSP);
	//#endif
	pac[idx++] = 0x06;
	pac[idx++] = LSBYTEOF16BIT(Conn_Handle);
	pac[idx++] = MSBYTEOF16BIT(Conn_Handle);
	//	if(NOOFHANDLES<1)
	//	{
	//		pac[idx++] = LSBYTEOF16BIT(Handle_ID_Main + 3);
	//		pac[idx++] = MSBYTEOF16BIT(Handle_ID_Main + 3);
	//	}
	//	else
	//	{
	//		pac[idx++] = LSBYTEOF16BIT(Handle_ID_Main + 7);
	//		pac[idx++] = MSBYTEOF16BIT(Handle_ID_Main + 7);
	//	}

	pac[idx++] = LSBYTEOF16BIT(0x00);
	pac[idx++] = LSBYTEOF16BIT(0x01);
	pac[idx++] = 0x01;
	pac[idx++] = 0x00;

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
	return status;
}


void bond()
{
	char pac[550] = {};
	int i,idx=0;
	char bond[500] = {0X01,0X0B,0XFE,0XA0,0X00,0X00,0X04,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XBD,0X1A,0X3C,0XCD,0XA6,0XB8,0X99,0X58,0X99,0XB7,0X40,0XEB,0X7B,0X60,0XFF,0X4A,0X50,0X3F,0X10,0XD2,0XE3,0XB3,0XC9,0X74,0X38,0X5F,0XC5,0XA3,0XD4,0XF6,0X49,0X3F,0XE6,0X9D,0X35,0X0E,0X48,0X01,0X03,0XCC,0XDB,0XFD,0XF4,0XAC,0X11,0X91,0XF4,0XEF,0XB9,0XA5,0XF9,0XE9,0XA7,0X83,0X2C,0X5E,0X2C,0XBE,0X97,0XF2,0XD2,0X03,0XB0,0X20,0X8B,0XD2,0X89,0X15,0XD0,0X8E,0X1C,0X74,0X24,0X30,0XED,0X8F,0XC2,0X45,0X63,0X76,0X5C,0X15,0X52,0X5A,0XBF,0X9A,0X32,0X63,0X6D,0XEB,0X2A,0X65,0X49,0X9C,0X80,0XDC,0X01,0X10,0X77,0X00,0X03,0X00,0X01,0X10,0X77};

	for(i = 0; i < 164; i++)
	{
		pac[idx++] = bond[i];
	}

	BLECOMWrite(pac, idx);

	sleep(2);
	passkey();
	sleep(2);
	notify();
	sleep(2);
	data_led();
	sleep(2);
	while(1);
}

void notify()
{
	char data[12] = {0X01,0X92,0XFD,0X06,0X00,0X00,0X0C,0X00,0X01,0X00};
	char pac[50] = {};
	int i = 0, idx = 0;
	for(i = 0; i < 10; i++)
	{
		pac[idx++] = data[i];
	}
	BLECOMWrite(pac, idx);
}

void data_led()
{
	char data[50] = {0X01,0X92,0XFD,0X09,0X00,0X00,0X0E,0X00,0X3C,0X30,0X41,0X30,0X3E};
	char pac[50] = {};
	int i,idx;

	for(i = 0; i < 13; i++)
	{
		pac[idx++] = data[i];
	}
	BLECOMWrite(pac, idx);
}


void passkey()
{
	char passkey[15] = {0X01,0X0C,0XFE,0X08,0X00,0X00,0X36,0X35,0X34,0X33,0X32,0X31};
	char pac[50] = {};
	int i,idx=0;

	for(i = 0; i < 12; i++)
	{
		pac[idx++] = passkey[i];
	}

	BLECOMWrite(pac, idx);
}

int Enable_Notification_Rsp()
{
	int i=0;
	int status = -1;
	for(i=0;i<4;i++)
	{
		printf("WAITING FOR NOTIFICATION RSP\n");
		sys_mq_recv(WRITE_RSP_ID, buffer, &size);
		printf("Getting Some thing\n");
		if(buffer[0] == 0x04 && buffer[1] == 0xFF)
		{
			if(MAKE16OFXY(buffer[4],buffer[3]) == VS_COMMAND_STATUS)
			{
				if(MAKE16OFXY(buffer[7],buffer[6]) == 0xFD92)
				{
					if(buffer[5] == 0x00 )
					{
						return 0;
					}
				}
			}
			else
				status = -1;
		}
		else
			return 5;
	}

	return status;

}

int Update_Conn_Status(char *State)
{
	int i=0;

	//	Set_Data_Length();

	printf("===================================================================================================\n");
	printf("BLE_MAC = ");
	for(i=0;i<6;i++)
	{
		Conn_Device_List[Conn_Handle].mac[i]=Scanned_MAC_Buff[i];
		printf("%02X:",Conn_Device_List[Conn_Handle].mac[i]);
	}
	printf("\nCentral_ID = ");
	for(i=0;i<6;i++)
	{
		Conn_Device_List[Conn_Handle].Central_ID[i] = Master_ID_1[i];
		printf("%02X:",Conn_Device_List[Conn_Handle].Central_ID[i]);
	}
	Conn_Device_List[Conn_Handle].Conn_Handle = Conn_Handle;

	Conn_Device_List[Conn_Handle].Handle_id = Handle_ID_Main;
	Conn_Device_List[Conn_Handle].status = 1;
	Conn_Device_List[Conn_Handle].Device_State = State;

	printf("\nConn_Handle = %04X\n",Conn_Device_List[Conn_Handle].Conn_Handle);
	printf("Handle_ID = %04X\n",Conn_Device_List[Conn_Handle].Handle_id);
	printf("Status = %d\n",Conn_Device_List[Conn_Handle].status);
	printf("Device_State = %s\n",Conn_Device_List[Conn_Handle].Device_State);
	printf("===================================================================================================\n");

	//GAP_MAKE_DISCOVERABLE();

	return 0;
}

int Set_Data_Length()
{
	uint8_t pac[100] = {};
	int idx=0,status=-1,i=0;
	uint16_t OPCODE_SETDATA_LENGTH = 0x2022;
	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(OPCODE_SETDATA_LENGTH);
	pac[idx++] = MSBYTEOF16BIT(OPCODE_SETDATA_LENGTH);
	pac[idx++] = 0x00 + 6;  // 16BYTE SUUID +  Byte configuration ; ;
	pac[idx++] = LSBYTEOF16BIT(Handle_ID_Main + 5);
	pac[idx++] = MSBYTEOF16BIT(Handle_ID_Main + 5);
	pac[idx++] = 0x36;	// TxOctets - 54 Bytes
	pac[idx++] = 0x00;
	pac[idx++] = 0x48;	// 2120 - TxTime
	pac[idx++] = 0x08;

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
	return status;
}


int Device_conn_count()
{

	int i=0,Device_Conn_Count=0;
	for(i=0;i<4;i++)
	{
		if(Conn_Device_List[i].status == 1)
		{
			Device_Conn_Count++;
		}
	}

	return Device_Conn_Count;
}

void print_buffer(unsigned char *buffer, int size)
{
#if 1
	int idx;
	printf("{");
	for (idx = 0; idx < size; idx++)
		printf("0x%02x, ", buffer[idx]);
	printf("}");
	printf("\n");
#endif
	unsigned char *ptr = calloc(sizeof(unsigned char), ((size*3)+2));
	if (ptr != NULL) {
		free(ptr);
	}
}

int HCIExt_SetMaxDataLength(void)
{
	uint8_t pac[16] = {0};
	int idx = 0, status = -1;
	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(VS_HCI_EXTENSION_SET_MAX_DATA_LENGTH);	// OpCode: 0xFC21
	pac[idx++] = MSBYTEOF16BIT(VS_HCI_EXTENSION_SET_MAX_DATA_LENGTH);
	pac[idx++] = 0x08;
	pac[idx++] = LSBYTEOF16BIT(APP_TX_PDU_SIZE);
	pac[idx++] = MSBYTEOF16BIT(APP_TX_PDU_SIZE);
	pac[idx++] = LSBYTEOF16BIT(APP_TX_TIME);
	pac[idx++] = MSBYTEOF16BIT(APP_TX_TIME);
	pac[idx++] = LSBYTEOF16BIT(APP_RX_PDU_SIZE);
	pac[idx++] = MSBYTEOF16BIT(APP_RX_PDU_SIZE);
	pac[idx++] = LSBYTEOF16BIT(APP_RX_TIME);
	pac[idx++] = MSBYTEOF16BIT(APP_RX_TIME);

	printf("<~~~~~~~~~~~>\n");
	print_buffer(pac, idx);

	if (BLECOMWrite(pac, idx) == idx) {
		status = 0;
	}
	else {
		status = -1;
	}
	return status;
}

void PARAM_UPDATE()
{

	//	HCI_SetTxPower();				// 5_DBM

	SET_PARAM(0x0C, 0x1F40); 			// TGAP_CONN_SCAN_INT
	usleep(100);
	SET_PARAM(0x0D, 0x1F40);			// TGAP_CONN_SCAN_WIND
	usleep(100);
	SET_PARAM(0x0E, 0x1F40);			// TGAP_CONN_HIGH_SCAN_INT
	usleep(100);
	SET_PARAM(0x0F, 0x1F40);			// TGAP_CONN_HIGH_SCAN_WIND
	usleep(100);
	SET_PARAM(0x10, 0x1F40);			// TGAP_GEN_DISC_SCAN_INT
	usleep(100);
	SET_PARAM(0x11, 0x1F40);			// TGAP_GEN_DISC_SCAN_WIND
	usleep(100);
	SET_PARAM(0x12, 0x1F40); 			// TGAP_LIM_DISC_SCAN_INT
	usleep(100);
	SET_PARAM(0x13, 0x1F40);			// TGAP_LIM_DISC_SCAN_WIND
	usleep(100);
	SET_PARAM(0x17, 0x1F40);			// TGAP_CONN_EST_SCAN_INT
	usleep(100);
	SET_PARAM(0x18, 0x1F40);			// TGAP_CONN_EST_SCAN_WIND
	usleep(100);
	SET_PARAM(0x02, 0x1388);			// TGAP_GEN_DISC_SCAN
	usleep(100);
	SET_PARAM(0x03, 0x1388);			// TGAP_LIM_DISC_SCAN
	usleep(100);

	//	HCIExt_SetMaxDataLength();
	//	usleep(100);
	//	HCIExt_SetlocalSupportedFeatures();
	sleep(1);
}

void HCIExt_SetlocalSupportedFeatures(void)
{
	uint8_t pac[16] = {0};
	int idx=0,status=-1;
	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(VS_HCI_EXTENSION_SET_LOCAL_SUPPORTED_FEATURES);// OpCode: 0xFC06 (VS_HCI_EXTENSION_SET_LOCAL_SUPPORTED_FEATURES)
	pac[idx++] = MSBYTEOF16BIT(VS_HCI_EXTENSION_SET_LOCAL_SUPPORTED_FEATURES);
	pac[idx++] = 0x08;

	if (BLECOMWrite(pac, 12) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
	return status;
}


void SET_PARAM(uint8_t PARAM_ID, uint16_t INTERVALS)
{
	uint8_t pac[10] = {}; 
	int idx=0,status=-1;
	pac[idx++] = PKT_COMMAND; 
	pac[idx++] = LSBYTEOF16BIT(VS_GAP_SET_PARAMETER);		// OpCode         : 0xFE30 (PARAM_UPDATE)
	pac[idx++] = MSBYTEOF16BIT(VS_GAP_SET_PARAMETER); 
	pac[idx++] = 0x03;
	pac[idx++] = PARAM_ID;
	pac[idx++] = LSBYTEOF16BIT(INTERVALS);		// 20ms
	pac[idx++] = MSBYTEOF16BIT(INTERVALS);

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
	return status;
}

int HCI_SetTxPower()
{
	uint8_t pac[10] = {}; 
	int idx=0,status=-1;
	pac[idx++] = PKT_COMMAND; 
	pac[idx++] = LSBYTEOF16BIT(VS_HCI_EXTENSION_SET_TX_POWER);		// OpCode         : 0xFDFC (GATT_AddService)
	pac[idx++] = MSBYTEOF16BIT(VS_HCI_EXTENSION_SET_TX_POWER); 
	pac[idx++] = 0x01;						
	pac[idx++] = 0x0C;							// 0x0C (12) = 5_DBM

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}

	return status;
}

int GET_MAC()
{
	uint8_t pac[10] = {};
	int idx=0,status=-1,i=0;

	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(BT_READ_BD_ADDR);
	pac[idx++] = MSBYTEOF16BIT(BT_READ_BD_ADDR);
	pac[idx++] = 0x00;

	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}

	return status;
}

void exit_func(int i)
{
	signal(SIGINT, exit_func);
	printf("\nBye Bye!!!\n");
	exit(0);
}

void BLE_MAC_GET()
{
	FILE *f_mac = fopen(BLE_MAC_FILE, "r");
	fread(MY_MAC,1,17,f_mac);
	fclose(f_mac);
	printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	printf("%s\n",MY_MAC);
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}

void *CentralModeThread(void *args)
{

	sleep(3);
	HCI_Reset();
	sleep(1);
	HCI_Init();
	sleep(1);
	PARAM_UPDATE();
	sleep(1);
	tCommandRespQId = sys_mq_init(COMMANDRESP_THREAD_QUEUE_KEY);


	static CentralState_new_t CentralState = STATE_CENTRAL_INIT;
	uint8_t ble_status=-1;

	while(1)
	{
		ble_status=-1;
		switch(CentralState)
		{
			case STATE_CENTRAL_INIT:
				CentralState = STATE_CENTRAL_DEVICE_SCAN_REQ;
				break;

			case STATE_CENTRAL_DEVICE_SCAN_REQ:
				ble_status = Device_Scan_Req();
				if(ble_status == 0)
				{
					printf("BLE_scan_rsp_success\n");
					CentralState = STATE_CENTRAL_DEVICE_SCAN_DATA;
				}
				else
					CentralState = STATE_CENTRAL_INIT;
				break;

			case STATE_CENTRAL_DEVICE_SCAN_DATA:
				ble_status = Device_Scan_Data();
				if(ble_status == 0)
				{
					printf("SUCCESS - - UUID_Not_Match\n");
				}
				else if(ble_status == 1)
				{
					printf("SUCCESS - - SCAN_END\n");
					sleep(1);
					CentralState = STATE_CENTRAL_DEVICE_SCAN_REQ;
				}
				else if(ble_status == 2)
				{
					printf("UUID_Match\n");
					CentralState = STATE_CENTRAL_DEVICE_SCAN_CANCEL;
				}
				else if(ble_status == -1)
				{
					printf("Error_PKT\n");
					CentralState = STATE_CENTRAL_DEVICE_SCAN_REQ;
				}
				break;

			case STATE_CENTRAL_DEVICE_SCAN_CANCEL:
				ble_status = Device_Scan_Cancel_Req();
				if(ble_status == 0)
				{
					printf("SCAN_CANCEL_REQ_SUCCESS\n");
					CentralState = STATE_CENTRAL_DEVICE_SCAN_CANCEL_RSP;
				}
				else
				{
					printf("SCAN_CANCEL_REQ_ERROR\n");
				}
				break;

			case STATE_CENTRAL_DEVICE_SCAN_CANCEL_RSP:
				ble_status = Device_Scan_Cancel_Rsp();
				if(ble_status == 0)
				{
					printf("SCAN_CANCEL_RSP_SUCCESS\n");
					CentralState = STATE_CENTRAL_DEVICE_ESTABLISH_LINK_REQ;
				}
				else
				{
					printf("SCAN_CANCEL_RSP_ERROR\n");
				}
				break;

			case STATE_CENTRAL_DEVICE_ESTABLISH_LINK_REQ:
				ble_status = Device_Establish_Link_Req();
				if(ble_status == 0)
				{
					printf("ESTABLISH_LINK_REQ_SUCCESS\n");
					CentralState = STATE_CENTRAL_DEVICE_ESTABLISH_LINK_RSP;
				}
				else
				{
					printf("ESTABLISH_LINK_REQ_NOT_SUCCESS\n");
				}
				break;

			case STATE_CENTRAL_DEVICE_ESTABLISH_LINK_RSP:
				ble_status = Device_Establish_Link_Rsp();
				if(ble_status == 0)
				{
					printf("ESTABLISH_SUCCESS\n");
					bond();
					while(1);
				}
				else
				{
					printf("ESTABLISH_NOT_SUCCESS\n");
				}
				break;

			case STATE_CENTRAL_ENABLE_NOTIFICATION:
				ble_status = Enable_Notification();
				if(ble_status == 0)
				{	
					printf("Enable Notification SUCCESS\n");
				}
				else
				{
					printf("Enable Notification ERROR\n");
				}
				break;
		}

	}

}

int Device_Scan_Req()
{
	uint8_t pac[10] = {}; 
	int idx=0,status=-1;

	if(Device_conn_count() < 4)
	{
		pac[idx++] = PKT_COMMAND;
		pac[idx++] = LSBYTEOF16BIT(VS_GAP_DEVICE_DISCOVERY_REQUEST); /* 0x04 Opcode LSB */
		pac[idx++] = MSBYTEOF16BIT(VS_GAP_DEVICE_DISCOVERY_REQUEST); /* 0xFE Opcode MSB */
		pac[idx++] = 0x03; /* Length */
		pac[idx++] = 0x03; /* Mode (0x03) - All */
		pac[idx++] = 0x01; /* Active_scan (0x01) - Enable */
		pac[idx++] = 0x00; /* WhiteList (0x00) - All */

		if (BLECOMWrite(pac, idx) == idx){
			status = Device_Scan_Rsp();
		}
		else {
			status = -1;
		}
		return status;
	}
	else
	{
		sleep(5);
		status = -1;
	}
}

int Device_Scan_Rsp()
{

	int status=-1;

	sys_mq_recv(tCommandRespQId, buffer, &size);

	if(buffer[0] == 0x04 && buffer[1] == 0xFF)
	{
		if(MAKE16OFXY(buffer[4],buffer[3]) == VS_COMMAND_STATUS)
		{
			if(MAKE16OFXY(buffer[7],buffer[6]) == VS_GAP_DEVICE_DISCOVERY_REQUEST)
			{
				if(buffer[5] == 0x00)
				{
					status = 0;
				}
				else
					status = -1;
			}
			else
				status = -1;
		}
	}
	else
		status = -1;
}

int Device_Scan_Data()
{
	printf("Device_scan_data\n");
	int status=-1,i=0,j=0,mac_success_count=0;
	sys_mq_recv(tCommandRespQId, buffer, &size);

	if(buffer[0] == 0x04 && buffer[1] == 0xFF)
	{
		if(MAKE16OFXY(buffer[4],buffer[3]) == VS_GAP_DEVICE_INFORMATION)   // 0x060D  --  Device scan information
		{
			int data_length=(int)buffer[15];
			if(MAKE16OFXY(buffer[16],buffer[17]) == 0x0201 && MAKE16OFXY(buffer[21],buffer[22]) == 0x9eca &&  MAKE16OFXY(buffer[23],buffer[24])== 0xdc24){
				printf("BLE_MAC = ");
				for(i=0,j=13;i<6;i++,j--)
				{
					Scanned_MAC_Buff[i]=buffer[j];
					printf("%02X:",Scanned_MAC_Buff[i]);
				}
				printf("\n");
				return 2;
				/*
				   for(j=0;j<4;j++)
				   {
				   if(Conn_Device_List[j].status == 1)
				   {
				   if(Scanned_MAC_Buff[0] == Conn_Device_List[j].mac[0] && Scanned_MAC_Buff[1] == Conn_Device_List[j].mac[1] && Scanned_MAC_Buff[2] == Conn_Device_List[j].mac[2] && Scanned_MAC_Buff[3] == Conn_Device_List[j].mac[3] && Scanned_MAC_Buff[4] == Conn_Device_List[j].mac[4] && Scanned_MAC_Buff[5] == Conn_Device_List[j].mac[5])
				   {
				   printf("SAME_MAC\n");
				   mac_success_count=1;
				   }
				   else
				   printf("MAC_NOT_SAME\n");
				   }
				   }
				   if(mac_success_count==0)
				   {
				   status = 2;
				   }
				   else
				   status = 0;
				   */			}
				//			else
				//			{
				//				status = 0;
				//			}
		}
		else if(MAKE16OFXY(buffer[4],buffer[3]) == VS_GAP_DEVICE_DISCOVERY)   // 0x0601  --  Device scan end
		{
			status = 1;
		}
		else
		{
			status = -1;
		}
	}

}

int Device_Scan_Cancel_Req()
{
	uint8_t pac[10] = {}; 
	int idx=0,status=-1;
	pac[idx++] = PKT_COMMAND;
	pac[idx++] = LSBYTEOF16BIT(VS_GAP_DEVICE_DISCOVERY_CANCEL); /* 0x05 Opcode LSB */
	pac[idx++] = MSBYTEOF16BIT(VS_GAP_DEVICE_DISCOVERY_CANCEL); /* 0xFE Opcode MSB */
	pac[idx++] = 0x00; /* Length */

	//        pthread_mutex_lock(&CommandMutex);
	if (BLECOMWrite(pac, idx) == idx){
		status = 0;
	}
	else {
		status = -1;
	}
	//        pthread_mutex_unlock(&CommandMutex);
	return status;
}

int Device_Scan_Cancel_Rsp()
{

	int status=-1;

	sys_mq_recv(tCommandRespQId, buffer, &size);

	if(buffer[0] == 0x04 && buffer[1] == 0xFF)
	{
		if(MAKE16OFXY(buffer[4],buffer[3]) == VS_GAP_DEVICE_DISCOVERY)
		{
			if(buffer[5] == 0x30)
			{
				status = 0;
			}
			else
				status = -1;
		}
	}
	else
		status = -1;
	return status;
}

int open_gpio()
{
	int status, fd;
	fd = open(GPIOFILENAME, O_RDWR);
	if (fd < 0) {
		printf("GPIO Opening Failed\n");
		return 0;
	}
	return fd;
}

int close_gpio(int fd)
{
	close(fd);
}

