/******************************************************************************
*
*  File Description 
*  ---------------------
* This file defines constants and functions for HCI layer.
* See Bluetooth Core v 4.0, Vol. 2, Part E.
* 
*******************************************************************************/

#ifndef __HCI_CONST_H_
#define __HCI_CONST_H_

#include "link_layer.h"
#include "hci_tl.h"
#include "compiler.h"

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part D: THREE-WIRE UART TRANSPORT LAYER -> 4.5 PACKET TYPE
 */
#define HCI_COMMAND_PKT     0x01
#define HCI_ACLDATA_PKT     0x02
#define HCI_SCODATA_PKT     0x03
#define HCI_EVENT_PKT       0x04
#define HCI_VENDOR_PKT      0xff

// 这个定义没查到 ???
typedef __packed struct _hci_uart_pckt{
  uint8_t type;
  uint8_t data[VARIABLE_SIZE];
} PACKED hci_uart_pckt;
#define HCI_HDR_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 5.4.1 HCI Command packet
 */
typedef __packed struct _hci_command_hdr{
  uint16_t opcode;		/* OCF & OGF */
  uint8_t  plen;
} PACKED hci_command_hdr;
#define HCI_COMMAND_HDR_SIZE  3

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 5.4.4 HCI Event packet
 */
typedef __packed struct _hci_event_pckt{
  uint8_t evt;
  uint8_t plen;
  uint8_t data[VARIABLE_SIZE];
} PACKED hci_event_pckt;
#define HCI_EVENT_HDR_SIZE  2

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 5.4.2 HCI ACL Data packets
 */
typedef __packed struct _hci_acl_hdr{
  uint16_t handle;		/* Handle & Flags(PB, BC) */
  uint16_t dlen;
} PACKED hci_acl_hdr;
#define HCI_ACL_HDR_SIZE  4

/* Link Control */
/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.1 LINK CONTROL COMMANDS
 * 在这一章节中能够找到 "For the Link Control commands, the OGF is defined as 0x01." 描述
 */
#define OGF_LINK_CTL      0x01

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.1.6 Disconnect command
 */
#define OCF_DISCONNECT    0x0006
typedef __packed struct _disconnect_cp{
  uint16_t handle;
  uint8_t  reason;
} PACKED disconnect_cp;
#define DISCONNECT_CP_SIZE  3

/* Host Controller and Baseband */
/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.3 CONTROLLER & BASEBAND COMMANDS
 * 在这一章节中能够找到 "For the HCI Control and Baseband commands, the OGF is defined as 0x03." 描述
 */
#define OGF_HOST_CTL            0x03

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 77.3.1 Set Event Mask command
 */
#define OCF_SET_EVENT_MASK      0x0001
/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.3.3 Set Event Filter command
 */
#define OCF_RESET               0x0003

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.3.35 Read Transmit Power Level command
 */
#define OCF_READ_TRANSMIT_POWER_LEVEL	0x002D
typedef __packed struct _read_transmit_power_level_cp{
  uint16_t handle;
  uint8_t  type;
} PACKED read_transmit_power_level_cp; // host 给 control 发送的命令报文
#define READ_TRANSMIT_POWER_LEVEL_CP_SIZE 3
typedef __packed struct _read_transmit_power_level_rp{
  uint8_t  status;
  uint16_t handle;
  int8_t   level;
} PACKED read_transmit_power_level_rp; // control 回复给 host 的数据报文
#define READ_TRANSMIT_POWER_LEVEL_RP_SIZE 4 // host 收到 control 发来的功率数据报文长度为 4 字节

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.3.38 Set Controller To Host Flow Control command
 */
#define OCF_SET_CONTROLLER_TO_HOST_FC   0x0031

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.3.39 Host Buffer Size command
 */
#define OCF_HOST_BUFFER_SIZE            0x0033

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.3.40 Host Number Of Completed Packets command
 */
#define OCF_HOST_NUM_COMP_PKTS          0x0035

/* Informational Parameters */
/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.4 INFORMATIONAL PARAMETERS
 * 在这一章节中能够找到 "For Informational Parameters commands, the OGF is defined as 0x04." 描述
 */
#define OGF_INFO_PARAM                  0x04

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.4.1 Read Local Version Information command
 */
#define OCF_READ_LOCAL_VERSION          0x0001
typedef __packed struct _read_local_version_rp{
  uint8_t  status;
  uint8_t  hci_version;
  uint16_t hci_revision;
  uint8_t  lmp_pal_version;
  uint16_t manufacturer_name;
  uint16_t lmp_pal_subversion;
} PACKED read_local_version_rp;
#define READ_LOCAL_VERSION_RP_SIZE      9

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.4.2 Read Local Supported Commands command
 */
#define OCF_READ_LOCAL_COMMANDS         0x0002

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.4.3 Read Local Supported Features command
 */
#define OCF_READ_LOCAL_FEATURES         0x0003

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.4.9 Read Local Simple Pairing Options command
 */
#define OCF_READ_BD_ADDR                0x0009
typedef __packed struct _read_bd_addr_rp{
  uint8_t status;
  tBDAddr bdaddr; // 6 字节
} PACKED read_bd_addr_rp;
#define READ_BD_ADDR_RP_SIZE            7

/* Status params */
/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.5 STATUS PARAMETERS
 * 在这一章节中能够找到 "For the status parameters commands, the OGF is defined as 0x05." 描述
 */
#define OGF_STATUS_PARAM                0x05

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.5.5 Read AFH Channel Map command
 */
#define OCF_READ_RSSI                   0x0005
typedef __packed struct _read_rssi_cp{
  uint16_t handle;
} PACKED read_rssi_cp; // host 发送给 control 的命令格式
#define READ_RSSI_CP_SIZE               2
typedef __packed struct _read_rssi_rp{
  uint8_t  status;
  uint16_t handle;
  int8_t   rssi;
} PACKED read_rssi_rp; // control 返回给 host 的数据格式
#define READ_RSSI_RP_SIZE               4


/* LE commands */
/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8 LE CONTROLLER COMMANDS
 * 在这一章节中能够找到 "For the LE Controller commands, the OGF code is defined as 0x08." 描述
 */
#define OGF_LE_CTL                      0x08

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.1 LE Set Event Mask command
 */
#define OCF_LE_SET_EVENT_MASK           0x0001
typedef __packed struct _le_set_event_mask_cp{
  uint8_t mask[8];
} PACKED le_set_event_mask_cp;
#define LE_SET_EVENT_MASK_CP_SIZE       8

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.2 LE Read Buffer Size command
 */
#define OCF_LE_READ_BUFFER_SIZE         0x0002
typedef __packed struct _le_read_buffer_size_rp{
  uint8_t  status;
  uint16_t pkt_len;
  uint8_t  max_pkt;
} PACKED le_read_buffer_size_rp;
#define LE_READ_BUFFER_SIZE_RP_SIZE     4

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.4 LE Set Random Address command
 */
#define OCF_LE_READ_LOCAL_SUPPORTED_FEATURES	0x0003
typedef __packed struct _le_read_local_supported_features_rp{
  uint8_t status;
  uint8_t features[8];
} PACKED le_read_local_supported_features_rp;
#define LE_READ_LOCAL_SUPPORTED_FEATURES_RP_SIZE 9

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.5 LE Set Advertising Parameters command
 */
#define OCF_LE_SET_RANDOM_ADDRESS		0x0005
typedef __packed struct _le_set_random_address_cp{
  tBDAddr bdaddr;
} PACKED le_set_random_address_cp;
#define LE_SET_RANDOM_ADDRESS_CP_SIZE 6

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.5 LE Set Advertising Parameters command
 */
#define OCF_LE_SET_ADV_PARAMETERS	0x0006
typedef __packed struct _le_set_adv_parameters_cp{
  uint16_t min_interval;
  uint16_t max_interval;
  uint8_t  advtype;
  uint8_t  own_bdaddr_type;
  uint8_t  direct_bdaddr_type;
  tBDAddr  direct_bdaddr;
  uint8_t  chan_map;
  uint8_t  filter;
} PACKED le_set_adv_parameters_cp;
#define LE_SET_ADV_PARAMETERS_CP_SIZE 15

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.6 LE Read Advertising Physical Channel Tx Power command
 */
#define OCF_LE_READ_ADV_CHANNEL_TX_POWER	0x0007
typedef __packed struct _le_read_adv_channel_tx_power_rp{
  uint8_t status;
  int8_t  level;
} PACKED le_read_adv_channel_tx_power_rp;
#define LE_READ_ADV_CHANNEL_TX_POWER_RP_SIZE 2

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.7 LE Set Advertising Data command
 */
#define OCF_LE_SET_ADV_DATA		0x0008
typedef __packed struct _le_set_adv_data_cp{
  uint8_t length;
  uint8_t data[31];
} PACKED le_set_adv_data_cp;
#define LE_SET_ADV_DATA_CP_SIZE 32

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.8 LE Set Scan Response Data command
 */
#define OCF_LE_SET_SCAN_RESPONSE_DATA		0x0009
typedef __packed struct _le_set_scan_response_data_cp{
  uint8_t length;
  uint8_t data[31];
} PACKED le_set_scan_response_data_cp;
#define LE_SET_SCAN_RESPONSE_DATA_CP_SIZE 32

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.9 LE Set Advertising Enable command
 */
#define OCF_LE_SET_ADVERTISE_ENABLE		0x000A
typedef __packed struct _le_set_advertise_enable_cp{
  uint8_t enable;
} PACKED le_set_advertise_enable_cp;
#define LE_SET_ADVERTISE_ENABLE_CP_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.10 LE Set Scan Parameters command
 */
#define OCF_LE_SET_SCAN_PARAMETERS		0x000B
typedef __packed struct _le_set_scan_parameters_cp{
  uint8_t  type;
  uint16_t interval;
  uint16_t window;
  uint8_t  own_bdaddr_type;
  uint8_t  filter;
} PACKED le_set_scan_parameters_cp;
#define LE_SET_SCAN_PARAMETERS_CP_SIZE 7

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.11 LE Set Scan Enable command
 */
#define OCF_LE_SET_SCAN_ENABLE			0x000C
typedef __packed struct _le_set_scan_enable_cp{
  uint8_t enable;
  uint8_t filter_dup;
} PACKED le_set_scan_enable_cp;
#define LE_SET_SCAN_ENABLE_CP_SIZE 2

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.12 LE Create Connection command
 */
#define OCF_LE_CREATE_CONN			0x000D
typedef __packed struct _le_create_connection_cp{
  uint16_t interval;
  uint16_t window;
  uint8_t  initiator_filter;
  uint8_t  peer_bdaddr_type;
  tBDAddr  peer_bdaddr;
  uint8_t  own_bdaddr_type;
  uint16_t min_interval;
  uint16_t max_interval;
  uint16_t latency;
  uint16_t supervision_timeout;
  uint16_t min_ce_length;
  uint16_t max_ce_length;
} PACKED le_create_connection_cp;
#define LE_CREATE_CONN_CP_SIZE 25

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.13 LE Create Connection Cancel command
 */
#define OCF_LE_CREATE_CONN_CANCEL		0x000E

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.14 LE Read Filter Accept List Size command
 */
#define OCF_LE_READ_WHITE_LIST_SIZE		0x000F
typedef __packed struct _le_read_white_list_size_rp{
  uint8_t status;
  uint8_t size;
} PACKED le_read_white_list_size_rp;
#define LE_READ_WHITE_LIST_SIZE_RP_SIZE 2

/* 具体结构体定义通过如下方式查询 --- 这里并未定义相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.15 LE Clear Filter Accept List command
 */
#define OCF_LE_CLEAR_WHITE_LIST			0x0010

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.16 LE Add Device To Filter Accept List command
 */
#define OCF_LE_ADD_DEVICE_TO_WHITE_LIST		0x0011
typedef __packed struct _le_add_device_to_white_list_cp{
  uint8_t bdaddr_type;
  tBDAddr bdaddr;
} PACKED le_add_device_to_white_list_cp;
#define LE_ADD_DEVICE_TO_WHITE_LIST_CP_SIZE 7

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.17 LE Remove Device From Filter Accept List command
 */
#define OCF_LE_REMOVE_DEVICE_FROM_WHITE_LIST	0x0012
typedef __packed struct _le_remove_device_from_white_list_cp{
  uint8_t bdaddr_type;
  tBDAddr bdaddr;
} PACKED le_remove_device_from_white_list_cp;
#define LE_REMOVE_DEVICE_FROM_WHITE_LIST_CP_SIZE 7

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.18 LE Connection Update command
 */
#define OCF_LE_CONN_UPDATE			0x0013
typedef __packed struct _le_connection_update_cp{
  uint16_t handle;
  uint16_t min_interval;
  uint16_t max_interval;
  uint16_t latency;
  uint16_t supervision_timeout;
  uint16_t min_ce_length;
  uint16_t max_ce_length;
} PACKED le_connection_update_cp;
#define LE_CONN_UPDATE_CP_SIZE 14

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.19 LE Set Host Channel Classification command
 */
#define OCF_LE_SET_HOST_CHANNEL_CLASSIFICATION	0x0014
typedef __packed struct _le_set_host_channel_classification_cp{
  uint8_t map[5];
} PACKED le_set_host_channel_classification_cp;
#define LE_SET_HOST_CHANNEL_CLASSIFICATION_CP_SIZE 5

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.20 LE Read Channel Map command
 */
#define OCF_LE_READ_CHANNEL_MAP			0x0015
typedef __packed struct _le_read_channel_map_cp{
  uint16_t handle;
} PACKED le_read_channel_map_cp; // host 发送给 control 的 Command parameters
#define LE_READ_CHANNEL_MAP_CP_SIZE 2
typedef __packed struct _le_read_channel_map_rp{
  uint8_t  status;
  uint16_t handle;
  uint8_t  map[5];
} PACKED le_read_channel_map_rp; // control 返回给 host 的 Return parameters
#define LE_READ_CHANNEL_MAP_RP_SIZE 8

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.21 LE Read Remote Features command
 */
#define OCF_LE_READ_REMOTE_USED_FEATURES	0x0016
typedef __packed struct _le_read_remote_used_features_cp{
  uint16_t handle;
} PACKED le_read_remote_used_features_cp;
#define LE_READ_REMOTE_USED_FEATURES_CP_SIZE 2

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.22 LE Encrypt command
 */
#define OCF_LE_ENCRYPT				0x0017
typedef __packed struct _le_encrypt_cp{
  uint8_t key[16];
  uint8_t plaintext[16];
} PACKED le_encrypt_cp; // host 发送给 control 的 Command parameters
#define LE_ENCRYPT_CP_SIZE 32
typedef __packed struct _le_encrypt_rp{
  uint8_t status;
  uint8_t encdata[16];
} PACKED le_encrypt_rp; // control 返回给 host 的 Return parameters
#define LE_ENCRYPT_RP_SIZE 17

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.23 LE Rand command
 */
#define OCF_LE_RAND				0x0018
typedef __packed struct _le_rand_rp{
  uint8_t status;
  uint8_t random[8];
} PACKED le_rand_rp; // host 发送给 control 的 Command parameters
#define LE_RAND_RP_SIZE 9

#define OCF_LE_START_ENCRYPTION			0x0019
typedef __packed struct _le_start_encryption_cp{
  uint16_t handle;
  uint8_t  random[8];
  uint16_t diversifier;
  uint8_t  key[16];
} PACKED le_start_encryption_cp; // control 返回给 host 的 Return parameters
#define LE_START_ENCRYPTION_CP_SIZE 28

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.25 LE Long Term Key Request Reply command
 */
#define OCF_LE_LTK_REPLY			0x001A
typedef __packed struct _le_ltk_reply_cp{
  uint16_t handle;
  uint8_t  key[16];
} PACKED le_ltk_reply_cp; // host 发送给 control 的 Command parameters
#define LE_LTK_REPLY_CP_SIZE 18

typedef __packed struct _le_ltk_reply_rp{
  uint8_t  status;
  uint16_t handle;
} PACKED le_ltk_reply_rp; // control 返回给 host 的 Return parameters
#define LE_LTK_REPLY_RP_SIZE 3

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.26 LE Long Term Key Request Negative Reply command
 */
#define OCF_LE_LTK_NEG_REPLY			0x001B
typedef __packed struct _le_ltk_neg_reply_cp{
  uint16_t handle;
} PACKED le_ltk_neg_reply_cp; // host 发送给 control 的 Command parameters
#define LE_LTK_NEG_REPLY_CP_SIZE 2

typedef __packed struct _le_ltk_neg_reply_rp{
  uint8_t  status;
  uint16_t handle;
} PACKED le_ltk_neg_reply_rp; // control 返回给 host 的 Return parameters
#define LE_LTK_NEG_REPLY_RP_SIZE 3

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.27 LE Read Supported States command
 */
#define OCF_LE_READ_SUPPORTED_STATES		0x001C
typedef __packed struct _le_read_supported_states_rp{
  uint8_t status;
  uint8_t states[8];
} PACKED le_read_supported_states_rp;
#define LE_READ_SUPPORTED_STATES_RP_SIZE 9

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.28 LE Receiver Test command
 */
#define OCF_LE_RECEIVER_TEST			0x001D
typedef __packed struct _le_receiver_test_cp{
  uint8_t frequency;
} PACKED le_receiver_test_cp;
#define LE_RECEIVER_TEST_CP_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.29 LE Transmitter Test command
 */
#define OCF_LE_TRANSMITTER_TEST			0x001E
typedef __packed struct _le_transmitter_test_cp{
  uint8_t frequency;
  uint8_t length;
  uint8_t payload;
} PACKED le_transmitter_test_cp;
#define LE_TRANSMITTER_TEST_CP_SIZE 3

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.8.30 LE Test End command
 */
#define OCF_LE_TEST_END				0x001F
typedef __packed struct _le_test_end_rp{
  uint8_t  status;
  uint16_t num_pkts;
} PACKED le_test_end_rp;
#define LE_TEST_END_RP_SIZE 3

/* 特定于厂商的命令 */
#define OGF_VENDOR_CMD		0x3f


/*------------- Events -------------*/
/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.3 Connection Complete event
 */
#define EVT_CONN_COMPLETE		0x03
typedef __packed struct _evt_conn_complete{
  uint8_t  status;
  uint16_t handle;
  tBDAddr  bdaddr; // 6 byte
  uint8_t  link_type;
  uint8_t  encr_mode;
} PACKED evt_conn_complete;
#define EVT_CONN_COMPLETE_SIZE 13 // 连接完成事件数据包长度（这里为什么是13字节，而不是11字节，不太明白）

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.5 Disconnection Complete event
 */
#define EVT_DISCONN_COMPLETE		0x05
typedef __packed struct _evt_disconn_complete{
  uint8_t  status;
  uint16_t handle;
  uint8_t  reason;
} PACKED evt_disconn_complete;
#define EVT_DISCONN_COMPLETE_SIZE 4 // 连接断联事件数据包长度

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.8 Encryption Change event
 */
#define EVT_ENCRYPT_CHANGE		0x08
typedef __packed struct _evt_encrypt_change{
  uint8_t  status;
  uint16_t handle;
  uint8_t  encrypt;
} PACKED evt_encrypt_change;
#define EVT_ENCRYPT_CHANGE_SIZE 5 // 加密更改事件数据包长度（这里为什么是5字节，而不是1+2+1=4字节，不太明白）

/* 具体结构体定义通过如下方式查询---这个地方没有添加相关结构体
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.12 Read Remote Version Information Complete event
 */
#define EVT_READ_REMOTE_VERSION_COMPLETE	0x0C

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.14 Command Complete event
 */
#define EVT_CMD_COMPLETE 		0x0E
typedef __packed struct _evt_cmd_complete{
  uint8_t  ncmd;
  uint16_t opcode;
} PACKED evt_cmd_complete;
#define EVT_CMD_COMPLETE_SIZE 3 // HCI 命令完成事件数据包长度

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.15 Command Status event
 */
#define EVT_CMD_STATUS 			0x0F
typedef __packed struct _evt_cmd_status{
  uint8_t  status;
  uint8_t  ncmd;
  uint16_t opcode;
} PACKED evt_cmd_status;
#define EVT_CMD_STATUS_SIZE 4 // 命令状态事件数据包长度

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.16 Hardware Error event
 */
#define EVT_HARDWARE_ERROR		0x10
typedef __packed struct _evt_hardware_error{
  uint8_t code;
} PACKED evt_hardware_error;
#define EVT_HARDWARE_ERROR_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.19 Number Of Completed Packets event
 */
#define EVT_NUM_COMP_PKTS		0x13
typedef __packed struct _evt_num_comp_pkts{
  uint8_t num_hndl;
  /* variable length part */
} PACKED evt_num_comp_pkts;
#define EVT_NUM_COMP_PKTS_SIZE 1

/* evt_num_comp_pkts 的可变长度部分 */
typedef __packed struct _evt_num_comp_pkts_param{
  uint16_t hndl;
  uint16_t num_comp_pkts;
} PACKED evt_num_comp_pkts_param;
#define EVT_NUM_COMP_PKTS_PARAM_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.26 Data Buffer Overflow event
 */
#define EVT_DATA_BUFFER_OVERFLOW		0x1A
typedef __packed struct _evt_data_buffer_overflow{
  uint8_t link_type;
} PACKED evt_data_buffer_overflow;
#define EVT_DATA_BUFFER_OVERFLOW_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.39 Encryption Key Refresh Complete event
 */
#define EVT_ENCRYPTION_KEY_REFRESH_COMPLETE	0x30
typedef __packed struct _evt_encryption_key_refresh_complete{
  uint8_t  status;
  uint16_t handle;
} PACKED evt_encryption_key_refresh_complete;
#define EVT_ENCRYPTION_KEY_REFRESH_COMPLETE_SIZE 3

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.65 LE Meta event
 */
#define EVT_LE_META_EVENT	0x3E
typedef __packed struct _evt_le_meta_event{
  uint8_t subevent;
  uint8_t data[VARIABLE_SIZE];
} PACKED evt_le_meta_event;
#define EVT_LE_META_EVENT_SIZE 1

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.65.1 LE Connection Complete event
 */
#define EVT_LE_CONN_COMPLETE	0x01
typedef __packed struct _evt_le_connection_complete{
  uint8_t  status;
  uint16_t handle;
  uint8_t  role;
  uint8_t  peer_bdaddr_type;
  tBDAddr  peer_bdaddr;
  uint16_t interval;
  uint16_t latency;
  uint16_t supervision_timeout;
  uint8_t  master_clock_accuracy;
} PACKED evt_le_connection_complete;
#define EVT_LE_CONN_COMPLETE_SIZE 18

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.65.2 LE Advertising Report event
 */
#define EVT_LE_ADVERTISING_REPORT	0x02
typedef __packed struct _le_advertising_info{
  uint8_t evt_type;
  uint8_t bdaddr_type;
  tBDAddr bdaddr;
  uint8_t data_length;
  uint8_t data_RSSI[VARIABLE_SIZE]; // RSSI is last octect (signed integer).
} PACKED le_advertising_info;
#define LE_ADVERTISING_INFO_SIZE 9

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.65.3 LE Connection Update Complete event
 */
#define EVT_LE_CONN_UPDATE_COMPLETE	0x03
typedef __packed struct _evt_le_connection_update_complete{
  uint8_t  status;
  uint16_t handle;
  uint16_t interval;
  uint16_t latency;
  uint16_t supervision_timeout;
} PACKED evt_le_connection_update_complete;
#define EVT_LE_CONN_UPDATE_COMPLETE_SIZE 9

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.65.4 LE Read Remote Features Complete event
 */
#define EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE	0x04
typedef __packed struct _evt_le_read_remote_used_features_complete{
  uint8_t  status;
  uint16_t handle;
  uint8_t  features[8]; // 具体定义在 core 5.4 的 2844 页
} PACKED evt_le_read_remote_used_features_complete;
#define EVT_LE_READ_REMOTE_USED_FEATURES_COMPLETE_SIZE 11

/* 具体结构体定义通过如下方式查询
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 7.7.65.5 LE Long Term Key Request event
 */
#define EVT_LE_LTK_REQUEST	0x05
typedef __packed struct _evt_le_long_term_key_request{
  uint16_t handle;
  uint8_t  random[8];
  uint16_t ediv;
} PACKED evt_le_long_term_key_request;
#define EVT_LE_LTK_REQUEST_SIZE 12

/**
 * @brief 在 @ref hci_event_pckt 结构中定义的事件代码。如果事件代码是 EVT_VENDOR，
 * 应用程序可以使用 @ref evt_blue_aci 结构解析数据包。
 */
#define EVT_VENDOR	0xFF


/* 具体结构体定义通过如下方式查询，组成一个操作码(opcode)
 * core 5.4 -> vol4: Host Controller Interface -> 
 * Part E: HOST CONTROLLER INTERFACE FUNCTIONAL SPECIFICATION -> 
 * 5.4.1 HCI Command packet
 * 其中 OGF 占用高 6bit 字节，OCF 占用低 10bit 字节
 */
#define cmd_opcode_pack(ogf, ocf)   (uint16_t)((ocf & 0x03ff)|(ogf << 10))
#define cmd_opcode_ogf(op)          (op >> 10)
#define cmd_opcode_ocf(op)          (op & 0x03ff)

typedef enum {
  WAITING_TYPE,
  WAITING_OPCODE1,
  WAITING_OPCODE2,
  WAITING_EVENT_CODE,
  WAITING_HANDLE,
  WAITING_HANDLE_FLAG,
  WAITING_PARAM_LEN,
  WAITING_DATA_LEN1,
  WAITING_DATA_LEN2,
  WAITING_PAYLOAD
} hci_state;

typedef void (*hci_packet_complete_callback)(void *pckt, uint16_t len);

#endif /* __HCI_CONST_H_ */
