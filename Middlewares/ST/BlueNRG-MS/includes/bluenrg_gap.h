/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : bluenrg_gap.h
* Author             : AMS - HEA&RF BU
* Version            : V1.0.0
* Date               : 19-July-2012
* Description        : Header file for BlueNRG's GAP layer. 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************

广播包的数据结构体，各位可以学习： https://zyxbeyourself.blog.csdn.net/article/details/140468034

*******************************************************************************/
#ifndef __BLUENRG_GAP_H__
#define __BLUENRG_GAP_H__

#include <link_layer.h>

/** 
 * @addtogroup HIGH_LEVEL_INTERFACE HIGH_LEVEL_INTERFACE
 * @{
 */
 
/**
 * @addtogroup ACI_GAP ACI_GAP
 * @brief API for GAP layer.
 * @{
 */

/**
 * @name GAP UUIDs
 * @{
 * 相关描述请查阅 Assigned_Numbers.pdf
 */
#define GAP_SERVICE_UUID                        (0x1800)
#define DEVICE_NAME_UUID						(0x2A00)
#define APPEARANCE_UUID							(0x2A01)
#define PERIPHERAL_PRIVACY_FLAG_UUID			(0x2A02)
#define RECONNECTION_ADDR_UUID					(0x2A03)
#define PERIPHERAL_PREFERRED_CONN_PARAMS_UUID	(0x2A04)
/**
 * @}
 */

/**
 * @name Characteristic value lengths
 * @{
 */
/* core 5.4 -> vol 3: Host -> Part C: GENERIC ACCESS PROFILE -> 12.1 DEVICE NAME CHARACTERISTIC
 * 存在描述 : "The Device Name characteristic value shall be 0 to 248 octets in length"
 * 这里规定 8 字节应该是为了让设计更加简单? 也许，我猜是这样的
 */
#define DEVICE_NAME_CHARACTERISTIC_LEN			        (8)
/* core 5.4 -> vol 3: Host -> Part C: GENERIC ACCESS PROFILE -> 12.2 APPEARANCE CHARACTERISTIC
 * 存在描述 : "The Appearance characteristic value shall be 2 octets in length"
 */
#define APPEARANCE_CHARACTERISTIC_LEN			        (2)
#define PERIPHERAL_PRIVACY_CHARACTERISTIC_LEN	        (1)
#define RECONNECTION_ADDR_CHARACTERISTIC_LEN			(6)
#define PERIPHERAL_PREF_CONN_PARAMS_CHARACTERISTIC_LEN	(8)
/**
 * @}
 */

/*------------- 广播数据和扫描响应数据的AD类型  ----------------*/

/** AD types
 * 具体结构定义通过如下方式查询
 * Assigned_Numbers.pdf -> 2.3 Common Data Types
 */
#define AD_TYPE_FLAGS							(0x01)
/** flag bits
 * 具体结构定义通过如下方式查询
 * CSS_v12.pdf -> Part A: DATA TYPES SPECIFICATION -> 1.3.2 Format
 */
#define FLAG_BIT_LE_LIMITED_DISCOVERABLE_MODE	(0x01)
#define FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE	(0x02)
#define FLAG_BIT_BR_EDR_NOT_SUPPORTED			(0x04)
#define FLAG_BIT_LE_BR_EDR_CONTROLLER			(0x08)
#define FLAG_BIT_LE_BR_EDR_HOST					(0x10)

/**
 * @name Service UUID AD types
 * 
 * 具体结构定义通过如下方式查询
 * CSS_v12.pdf -> Part A: DATA TYPES SPECIFICATION -> 1.1.2 Format
 */
#define AD_TYPE_16_BIT_SERV_UUID				(0x02)
#define AD_TYPE_16_BIT_SERV_UUID_CMPLT_LIST		(0x03)
#define AD_TYPE_32_BIT_SERV_UUID				(0x04)
#define AD_TYPE_32_BIT_SERV_UUID_CMPLT_LIST		(0x05)
#define AD_TYPE_128_BIT_SERV_UUID				(0x06)
#define AD_TYPE_128_BIT_SERV_UUID_CMPLT_LIST	(0x07)

/** AD types
 * 具体结构定义通过如下方式查询
 * Assigned_Numbers.pdf -> 2.3 Common Data Types
 */
#define AD_TYPE_SHORTENED_LOCAL_NAME			(0x08)
#define AD_TYPE_COMPLETE_LOCAL_NAME			    (0x09)
#define AD_TYPE_TX_POWER_LEVEL					(0x0A)
#define AD_TYPE_CLASS_OF_DEVICE					(0x0D)
#define AD_TYPE_SEC_MGR_TK_VALUE				(0x10)
#define AD_TYPE_SEC_MGR_OOB_FLAGS				(0x11)
#define AD_TYPE_SLAVE_CONN_INTERVAL				(0x12)
#define AD_TYPE_SERV_SOLICIT_16_BIT_UUID_LIST	(0x14)
#define AD_TYPE_SERV_SOLICIT_32_BIT_UUID_LIST	(0x1F)
#define AD_TYPE_SERV_SOLICIT_128_BIT_UUID_LIST	(0x15)
#define AD_TYPE_SERVICE_DATA					(0x16)
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA		(0xFF)

#define MAX_ADV_DATA_LEN						(31) // BLE 4.0 广播包最大载荷

#define DEVICE_NAME_LEN			  				(7)  // 设备名称长度，这个应该是该协议栈为了简单，就固定 7 字节
#define BD_ADDR_SIZE							(6)  // 蓝牙 MAC 地址长度为 6 字节

/**
 * @name Privacy flag values
 * @{
 */
#define PRIVACY_ENABLED							(0x01)
#define PRIVACY_DISABLED						(0x00)

/**
 * @name 广播间隔
 * 广播包最小单位为 625 微秒，这里定义了几种广播间隔时间
 * @{
 */
#define DIR_CONN_ADV_INT_MIN					(0x190)/*250ms*/
#define DIR_CONN_ADV_INT_MAX					(0x320)/*500ms*/
#define UNDIR_CONN_ADV_INT_MIN					(0x800)/*1.28s*/
#define UNDIR_CONN_ADV_INT_MAX					(0x1000)/*2.56s*/
#define LIM_DISC_ADV_INT_MIN					(0x190)/*250ms*/
#define LIM_DISC_ADV_INT_MAX					(0x320)/*500ms*/
#define GEN_DISC_ADV_INT_MIN					(0x800)/*1.28s*/
#define GEN_DISC_ADV_INT_MAX					(0x1000)/*2.56s*/

/**
 * @name Timeout values
 * @{
 */
#define LIM_DISC_MODE_TIMEOUT					(180000)/* 180 seconds. according to the errata published */
#define PRIVATE_ADDR_INT_TIMEOUT				(900000)/* 15 minutes */
/**
 * @}
 */

/**
 * @anchor gap_roles
 * @name GAP 角色
 * @{
 * 
 * 这个是用来定义 GAP 角色，具体值为什么是如下定义感觉不是 SIG 定义，而是 control 芯片定义的广播状态(猜测)
 * 相关 GAP 角色定义可以查阅
 * core 5.4 -> vol 3: Host -> Part C: GENERIC ACCESS PROFILE -> 2.2 PROFILE ROLES
 * 
 */
#define GAP_PERIPHERAL_ROLE_IDB05A1			(0x01)
#define GAP_BROADCASTER_ROLE_IDB05A1		(0x02)
#define GAP_CENTRAL_ROLE_IDB05A1			(0x04)
#define GAP_OBSERVER_ROLE_IDB05A1			(0x08)

#define GAP_PERIPHERAL_ROLE_IDB04A1			(0x01)
#define GAP_BROADCASTER_ROLE_IDB04A1		(0x02)
#define GAP_CENTRAL_ROLE_IDB04A1			(0x03)
#define GAP_OBSERVER_ROLE_IDB04A1			(0x04)

/**
 * @anchor gap_procedure_codes
 * @name GAP procedure codes
 * 用于 EVT_BLUE_GAP_PROCEDURE_COMPLETE 事件和
 * aci_gap_terminate_gap_procedure() 命令的过程代码
 * @{
 */
#define GAP_LIMITED_DISCOVERY_PROC                  (0x01) // 限制发现过程，用于设备在有限时间内可被发现
#define GAP_GENERAL_DISCOVERY_PROC                  (0x02) // 一般发现过程，用于设备在无限时间内可被发现
#define GAP_NAME_DISCOVERY_PROC                     (0x04) // 名称发现过程，用于获取设备名称
#define GAP_AUTO_CONNECTION_ESTABLISHMENT_PROC      (0x08) // 自动连接建立过程，用于自动连接到已知设备
#define GAP_GENERAL_CONNECTION_ESTABLISHMENT_PROC   (0x10) // 一般连接建立过程，用于连接到任意设备
#define GAP_SELECTIVE_CONNECTION_ESTABLISHMENT_PROC (0x20) // 选择性连接建立过程，用于连接到特定设备
#define GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC    (0x40) // 直接连接建立过程，用于快速连接到特定设备

#define GAP_OBSERVATION_PROC_IDB05A1                (0x80) // 观察过程，用于扫描广播数据（IDB05A1芯片）


#endif /* __BLUENRG_GAP_H__ */
