/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : hci.h
* Author             : AMG RF FW team
* Version            : V1.1.0
* Date               : 18-July-2016
* Description        : Header file for framework required for handling HCI interface.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __HCI_TL_H_
#define __HCI_TL_H_

#include "hci_tl_interface.h"
#include "bluenrg_types.h"
#include "ble_list.h"
#include "bluenrg_conf.h"

/** 
 * @addtogroup LOW_LEVEL_INTERFACE LOW_LEVEL_INTERFACE
 * @{
 */
  
/** 
 * @defgroup LL_HCI_TL HCI_TL
 * @{
 */

/** 
 * @defgroup BASIC BASIC
 * @{
 */
 
/** @defgroup BASIC_Types Exported Types
 * @{
 */ 

/**
 * @brief Structure hosting the HCI request
 * @{
 */ 
struct hci_request {
  uint16_t ogf;     /**< 操作码组字段（Opcode Group Field） */
  uint16_t ocf;     /**< 操作码命令字段（Opcode Command Field） */
  uint32_t event;   /**< HCI Event */
  void     *cparam; /**< HOST 发送给 control 的数据参数 */
  uint32_t clen;    /**< HOST 发送给 control 的数据长度 */
  void     *rparam; /**< control 返回给 HOST 的数据参数 */
  uint32_t rlen;    /**< control 返回给 HOST 的数据长度 */
};
/**
 * @}
 */
 
/**
 * @brief Structure used to read received HCI data packet
 * @{
 */
typedef struct _tHciDataPacket
{
  tListNode currentNode;
  uint8_t dataBuff[HCI_READ_PACKET_SIZE];
  uint8_t data_len;
} tHciDataPacket;
/**
 * @}
 */

/**
 * @brief Structure used to manage the BUS IO operations.
 *        All the structure fields will point to functions defined at user level.
 * @{
 */ 
typedef struct
{                
  int32_t (* Init)    (void* pConf); /**< 指向HCI TL函数的指针，用于IO总线初始化 */
  int32_t (* DeInit)  (void); /**< 指向HCI TL函数的指针，用于IO反总线初始化 */  
  int32_t (* Reset)   (void); /**< 指向HCI TL函数的指针，用于IO总线复位 */    
  int32_t (* Receive) (uint8_t*, uint16_t); /**< 指向用于IO总线数据接收的HCI TL函数的指针 */
  int32_t (* Send)    (uint8_t*, uint16_t); /**< 指向HCI TL函数的指针，用于IO总线数据传输 */
  int32_t (* DataAck) (uint8_t*, uint16_t* len); /**< 指向用于IO总线数据接收的HCI TL函数的指针 */	
  int32_t (* GetTick) (void); /**< 指向BSP函数的指针，用于获取HAL时间基时间戳 */    
} tHciIO;
/**
 * @}
 */

/**
 * @brief Describe the HCI flow status
 * @{
 */ 
typedef enum
{     
  HCI_DATA_FLOW_DISABLE = 0,
  HCI_DATA_FLOW_ENABLE,
} tHciflowStatus;
/**
 * @}
 */
 
/**
 * @brief Contain the HCI context
 * @{
 */
typedef struct
{   
  tHciIO io; /**< 管理BUS IO操作，也就是 SPI 的相关操作 */
  void (* UserEvtRx) (void * pData); /**< HCI 事件回调函数指针 */  
} tHciContext;

/**
 * @}
 */ 
 
/**
 * @}
 */
 
/** @defgroup BASIC_Functions Exported Functions
 * @{
 */

/**
  * @brief  Send an HCI request either in synchronous or in asynchronous mode.
  *
  * @param  r: The HCI request
  * @param  async: TRUE if asynchronous mode, FALSE if synchronous mode
  * @retval int: 0 when success, -1 when failure
  */
int hci_send_req(struct hci_request *r, BOOL async);
 
/**
 * @brief  Register IO bus services.
 *         The tHciIO structure is initialized here by assigning to each structure field a  
 *         function for managing the IO Bus.
 *         E.g. In case the user needs to register the SPI bus services:
 *         @code
           void hci_register_io_bus(tHciIO* fops)
           {			 
             hciContext.io.Init    = fops->Init; 
             hciContext.io.Receive = fops->Receive;  
             hciContext.io.Send    = fops->Send;
             hciContext.io.GetTick = fops->GetTick;
             hciContext.io.Reset   = fops->Reset;    
           }
 *         @endcode
 *         where:
 *         - hciContext is a static variable defined in the hci_tl.c
 *         - all fops fields are initialized, at user level (hci_tl_interface.c file), in the 
 *           void hci_tl_lowlevel_init(void) function. All the functions for managing the initialization,
 *           de-initialization, data sending/receiving, ... must be implemented by the user.
 *         e.g. 
 *         @code
           void hci_tl_lowlevel_init(void)
           {
             tHciIO fops;  
  
             //Register IO bus services 
             fops.Init    = HCI_TL_SPI_Init;
             fops.DeInit  = HCI_TL_SPI_DeInit;
             fops.Send    = HCI_TL_SPI_Send;
             fops.Receive = HCI_TL_SPI_Receive;
             fops.Reset   = HCI_TL_SPI_Reset;
             fops.GetTick = BSP_GetTick;
  
             hci_register_io_bus (&fops);
  
             //Register event irq handler
             ... ... ...
           }
 *         @endcode
 *            
 * @param  fops The HCI IO structure managing the IO BUS
 * @retval None
 */
void hci_register_io_bus(tHciIO* fops);
  
/**
 * @brief  Interrupt service routine that must be called when the BlueNRG 
 *         reports a packet received or an event to the host through the 
 *         BlueNRG-MS interrupt line.
 *
 * @param  pdata Packet or event pointer
 * @retval 0: packet/event processed, 1: no packet/event processed 
 */
int32_t hci_notify_asynch_evt(void* pdata);

/**
 * @brief  This function resume the User Event Flow which has been stopped on return 
 *         from UserEvtRx() when the User Event has not been processed.
 *
 * @param  None
 * @retval None
 */
void hci_resume_flow(void);

/**
 * @brief  This function is called when an ACI/HCI command is sent and the response 
 *         is waited from the BLE core.
 *         The application shall implement a mechanism to not return from this function 
 *         until the waited event is received.
 *         This is notified to the application with hci_cmd_resp_release().
 *         It is called from the same context the HCI command has been sent.
 *
 * @param  timeout: Waiting timeout
 * @retval None
 */
void hci_cmd_resp_wait(uint32_t timeout);

/**
 * @brief  This function is called when an ACI/HCI command is sent and the response is
 *         received from the BLE core.
 *
 * @param  flag: Release flag
 * @retval None
 */
void hci_cmd_resp_release(uint32_t flag);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */
 
#endif /* __HCI_TL_H_ */
