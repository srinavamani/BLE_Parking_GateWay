/*
 * BLEParameters.h
 *
 *  Created on: 06-Jun-2016
 *      Author: root
 */

#ifndef BLEPARAMETERS_H_
#define BLEPARAMETERS_H_

#ifndef MAX_NUM_PERIPHERALS
#define MAX_NUM_PERIPHERALS 5
#endif
#define RETRY_COUNT 4
#define DYNAMIC_LINK_LOGIC
#define DYNAMIC_CONNECTION_LIMIT    6 /* Should be less than Total No: of BLE connections */
#define INTERVAL 20
#define INTERVAL_2 180
#define NOTIFICATION_INTERVAL	10000

typedef enum {
#if 0  /* For reference */
	/*Minimum Time To Remain Advertising When In ,Discoverable Mode (mSec). Setting This
	Parameter To 0 Turns Off The Timer (default). TGAP_GEN_DISC_ADV_MIN*/
	TGAP_GEN_DISC_ADV_MIN,			//0
	/*Maximum Time To Remain Advertising, When In Limited Discoverable Mode (mSec). TGAP_LIM_ADV_TIMEOUT*/
	TGAP_LIM_ADV_TIMEOUT,
	/*Minimum Time To Perform Scanning, When Performing General Discovery Proc (mSec). TGAP_GEN_DISC_SCAN*/
	TGAP_GEN_DISC_SCAN,
	/*Minimum Time To Perform Scanning, When Performing Limited Discovery Proc (mSec). TGAP_LIM_DISC_SCAN*/
	TGAP_LIM_DISC_SCAN,
	/*Advertising Timeout, When Performing Connection Establishment Proc (mSec).TGAP_CONN_EST_ADV_TIMEOUT*/
	TGAP_CONN_EST_ADV_TIMEOUT,
	/*Link Layer Connection Parameter Update Notification Timer, Connection Parameter
	  Update Proc (mSec). TGAP_CONN_PARAM_TIMEOUT*/
	TGAP_CONN_PARAM_TIMEOUT,
	/*
	 * Minimum Advertising Interval, When In Limited Discoverable Mode (mSec). TGAP_LIM_DISC_ADV_INT_MIN
	 * */
	TGAP_LIM_DISC_ADV_INT_MIN,
	/*
	 * Maximum Advertising Interval, When In Limited Discoverable Mode (mSec). TGAP_LIM_DISC_ADV_INT_MAX
	 * */
	TGAP_LIM_DISC_ADV_INT_MAX,
	/*
	 *Minimum Advertising Interval, When In General
				  Discoverable Mode (mSec). TGAP_GEN_DISC_ADV_INT_MIN
	 * */
	TGAP_GEN_DISC_ADV_INT_MIN,

	/*
	 *Maximum Advertising Interval, When In General
				  Discoverable Mode (mSec). TGAP_GEN_DISC_ADV_INT_MAX
	 * */
	TGAP_GEN_DISC_ADV_INT_MAX,
	/*
	 *Minimum Advertising Interval, When In Connectable
				  Mode (mSec). TGAP_CONN_ADV_INT_MIN
	 * */
	TGAP_CONN_ADV_INT_MIN,
	/*
	 *Maximum Advertising Interval, When In Connectable
				  Mode (mSec). TGAP_CONN_ADV_INT_MAX
	 * */
	TGAP_CONN_ADV_INT_MAX,
	/*
	 *Scan Interval Used During Link Layer Initiating
				  State, When In Connectable Mode (mSec). TGAP_CONN_SCAN_INT
	 * */
	TGAP_CONN_SCAN_INT,
	/*
	 *Scan Window Used During Link Layer Initiating
				  State, When In Connectable Mode (mSec)TGAP_CONN_SCAN_WIND
	 * */
	TGAP_CONN_SCAN_WIND,
	/*
	 *Scan Interval Used During Link Layer Initiating
				  State, When In Connectable Mode, High Duty
				  Scan Cycle Scan Paramaters (mSec). TGAP_CONN_HIGH_SCAN_INT
	 * */
	TGAP_CONN_HIGH_SCAN_INT,
	/*
	 *Scan Window Used During Link Layer Initiating
				  State, When In Connectable Mode, High Duty
				  Scan Cycle Scan Paramaters (mSec). TGAP_CONN_HIGH_SCAN_WIND
	 * */
	TGAP_CONN_HIGH_SCAN_WIND,
	/*
	 *Scan Interval Used During Link Layer Scanning
				  State, When In General Discovery
				  Proc (mSec). TGAP_GEN_DISC_SCAN_INT
	 * */
	TGAP_GEN_DISC_SCAN_INT,
	/*
	 *Scan Window Used During Link Layer Scanning
				  State, When In General Discovery
				  Proc (mSec). TGAP_GEN_DISC_SCAN_WIND
	 * */
	TGAP_GEN_DISC_SCAN_WIND,
	/*
	 *Scan Interval Used During Link Layer Scanning
				  State, When In Limited Discovery
				  Proc (mSec). TGAP_LIM_DISC_SCAN_INT
	 * */
	TGAP_LIM_DISC_SCAN_INT,
	/*
	 *Scan Window Used During Link Layer Scanning
				  State, When In Limited Discovery
				  Proc (mSec). TGAP_LIM_DISC_SCAN_WIND
	 * */
	TGAP_LIM_DISC_SCAN_WIND,
	/*
	 *Advertising Interval, When Using Connection
				  Establishment Proc (mSec). TGAP_CONN_EST_ADV
	 * */
	TGAP_CONN_EST_ADV,
	/*
	 *Minimum Link Layer Connection Interval,
				  When Using Connection Establishment
				  Proc (mSec). TGAP_CONN_EST_INT_MIN
	 * */
	TGAP_CONN_EST_INT_MIN,
	/*
	 *Maximum Link Layer Connection Interval,
				  When Using Connection Establishment
				  Proc (mSec). TGAP_CONN_EST_INT_MAX
	 * */
	TGAP_CONN_EST_INT_MAX,
	/*
	 *Scan Interval Used During Link Layer Initiating
				  State, When Using Connection Establishment
				  Proc (mSec). TGAP_CONN_EST_SCAN_INT
	 * */
	TGAP_CONN_EST_SCAN_INT,
	/*
	 *Scan window Used During Link Layer Initiating
				  State, When Using Connection Establishment
				  Proc (mSec). TGAP_CONN_EST_SCAN_WIND
	 * */
	TGAP_CONN_EST_SCAN_WIND,
	/*
	 *Local Informational Parameter About Max Len
				  Of Connection Needed, When Using Connection
				  Establishment Proc (mSec). TGAP_CONN_EST_MAX_CE_LEN
	 * */
	TGAP_CONN_EST_MAX_CE_LEN,
	/*
	 *Minimum Time Interval Between Private
				  (Resolvable) Address Changes. In Minutes
				  (Default 15 Minutes) TGAP_PRIVATE_ADDR_INT
	 * */
	TGAP_PRIVATE_ADDR_INT,
	/*
	 *SM Message Timeout (Milliseconds).
				  (Default 30 Seconds). TGAP_SM_TIMEOUT
	 * */
	TGAP_SM_TIMEOUT,
	/*
	 *SM Minimum Key Length Supported
				  (default 7). TGAP_SM_MIN_KEY_LEN
	 * */
	TGAP_SM_MIN_KEY_LEN,
	/*
	 *SM Maximum Key Length Supported
				  (Default 16). TGAP_SM_MAX_KEY_LEN
	 * */
	TGAP_SM_MAX_KEY_LEN,
	/*
	 *
	 * */
	TGAP_FILTER_ADV_REPORTS,
	/*
	 *
	 * */
	TGAP_SCAN_RSP_RSSI_MIN,
	/*
	 *
	 * */
	SET_RX_DEBUG,
	/*
	 *Local Informational Parameter About Min Len
				  Of Connection Needed, When Using Connection
				   Establishment Proc (mSec). TGAP_CONN_EST_MIN_CE_LEN
	 * */
	TGAP_CONN_EST_MIN_CE_LEN,
	/*
	 *Link Layer Connection Slave Latency, When Using
				  Connection Establishment Proc (mSec) TGAP_CONN_EST_LATENCY
	 * */
	TGAP_CONN_EST_LATENCY,
	/*
	 *Link Layer Connection Supervision Timeout,
				  When Using Connection Establishment
				  Proc (mSec). TGAP_CONN_EST_SUPERV_TIMEOUT
	 * */
	TGAP_CONN_EST_SUPERV_TIMEOUT,
#else
	TGAP_GEN_DISC_ADV_MIN = 0x00,
	TGAP_LIM_ADV_TIMEOUT,
	TGAP_GEN_DISC_SCAN,
	TGAP_LIM_DISC_SCAN,
	TGAP_CONN_EST_ADV_TIMEOUT,
	TGAP_CONN_PARAM_TIMEOUT,
	TGAP_LIM_DISC_ADV_INT_MIN,
	TGAP_LIM_DISC_ADV_INT_MAX,
	TGAP_GEN_DISC_ADV_INT_MIN,
	TGAP_GEN_DISC_ADV_INT_MAX,
	TGAP_CONN_ADV_INT_MIN,
	TGAP_CONN_ADV_INT_MAX,
	TGAP_CONN_SCAN_INT,
	TGAP_CONN_SCAN_WIND,
	TGAP_CONN_HIGH_SCAN_INT,
	TGAP_CONN_HIGH_SCAN_WIND,
	TGAP_GEN_DISC_SCAN_INT,
	TGAP_GEN_DISC_SCAN_WIND,
	TGAP_LIM_DISC_SCAN_INT,
	TGAP_LIM_DISC_SCAN_WIND,
	TGAP_CONN_EST_ADV,
	TGAP_CONN_EST_INT_MIN,
	TGAP_CONN_EST_INT_MAX,
	TGAP_CONN_EST_SCAN_INT,
	TGAP_CONN_EST_SCAN_WIND,
	TGAP_CONN_EST_SUPERV_TIMEOUT,
	TGAP_CONN_EST_LATENCY,
	TGAP_CONN_EST_MIN_CE_LEN,
	TGAP_CONN_EST_MAX_CE_LEN,
	TGAP_PRIVATE_ADDR_INT,
	TGAP_SM_TIMEOUT,
	TGAP_SM_MIN_KEY_LEN,
	TGAP_SM_MAX_KEY_LEN,
	TGAP_FILTER_ADV_REPORTS,
	TGAP_SCAN_RSP_RSSI_MIN,
	SET_RX_DEBUG = 0xFE,
#endif
} BLEParameters_t;


#endif /* BLEPARAMETERS_H_ */
