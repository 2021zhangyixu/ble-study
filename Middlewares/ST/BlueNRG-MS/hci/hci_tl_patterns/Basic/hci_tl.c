/**
  ******************************************************************************
  * @file    hci_tl.c 
  * @author  AMG RF Application Team
  * @version V1.0.0
  * @date    18-Jan-2018
  * @brief   Contains the basic functions for managing the framework required
  *          for handling the HCI interface
  ******************************************************************************
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics</center></h2>
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/ 
#include "hci_const.h"
#include "hci.h"
#include "hci_tl.h"

#define HCI_LOG_ON                      0
#define HCI_PCK_TYPE_OFFSET             0
#define EVENT_PARAMETER_TOT_LEN_OFFSET  2

/**
 * 增加此参数以克服由于BLE设备拥挤的环境而可能出现的问题
 * 或来自外围设备的大量传入通知
 */
#define HCI_READ_PACKET_NUM_MAX 	   (5)

#define MIN(a,b)      ((a) < (b))? (a) : (b)
#define MAX(a,b)      ((a) > (b))? (a) : (b)

tListNode             hciReadPktPool;
tListNode             hciReadPktRxQueue;
static tHciDataPacket hciReadPacketBuffer[HCI_READ_PACKET_NUM_MAX];
static tHciContext    hciContext;

/************************* Static internal functions **************************/

/**
  * @brief  Verify the packet type.
  *
  * @param  hciReadPacket The HCI data packet
  * @retval 0: valid packet, 1: incorrect packet, 2: wrong length (packet truncated or too long)
  */
static int verify_packet(const tHciDataPacket * hciReadPacket)
{
  const uint8_t *hci_pckt = hciReadPacket->dataBuff;
  
  if (hci_pckt[HCI_PCK_TYPE_OFFSET] != HCI_EVENT_PKT)
    return 1; /* Incorrect type */
  
  if (hci_pckt[EVENT_PARAMETER_TOT_LEN_OFFSET] != hciReadPacket->data_len - (1+HCI_EVENT_HDR_SIZE))
    return 2; /* Wrong length (packet truncated or too long) */
  
  return 0;      
}

/**
  * @brief  发送 HCI 命令。
  *
  * @param  ogf 操作码组字段（Opcode Group Field）
  * @param  ocf 操作码命令字段（Opcode Command Field）
  * @param  plen HCI 命令的参数长度
  * @param  param HCI 命令的参数
  * @retval None
  */
static void send_cmd(uint16_t ogf, uint16_t ocf, uint8_t plen, void *param)
{
  uint8_t payload[HCI_MAX_PAYLOAD_SIZE]; // 定义用于存储 HCI 命令的缓冲区
  hci_command_hdr hc; // 定义 HCI 命令头结构
  
  // 将 OGF 和 OCF 打包成完整的操作码，并转换为小端格式
  hc.opcode = htobs(cmd_opcode_pack(ogf, ocf));
  hc.plen = plen; // 设置参数长度

  // 构建 HCI 命令包
  payload[0] = HCI_COMMAND_PKT; // 设置数据包类型为 HCI 命令包
  BLUENRG_memcpy(payload + 1, &hc, sizeof(hc)); // 将命令头复制到缓冲区
  BLUENRG_memcpy(payload + HCI_HDR_SIZE + HCI_COMMAND_HDR_SIZE, param, plen); // 将参数复制到缓冲区
  
  // 如果定义了发送函数，则通过底层接口发送命令
  if (hciContext.io.Send)
  {
    hciContext.io.Send (payload, HCI_HDR_SIZE + HCI_COMMAND_HDR_SIZE + plen);
  }
}

/**
  * @brief  Remove the tail from a source list and insert it to the head 
  *         of a destination list.
  *
  * @param  dest_list
  * @param  src_list
  * @retval None
  */
static void move_list(tListNode * dest_list, tListNode * src_list)
{
  pListNode tmp_node;
  
  while (!list_is_empty(src_list))
  {
    list_remove_tail(src_list, &tmp_node);
    list_insert_head(dest_list, tmp_node);
  }
}

/**
  * @brief  释放HCI事件列表
  *
  * @param  None
  * @retval None
  */
static void free_event_list(void)
{
  tHciDataPacket * pckt;
  
  // 当空闲的 HCI 数据包池中的节点数量小于最大数量的一半时，释放事件队列中的数据包
  while(list_get_size(&hciReadPktPool) < HCI_READ_PACKET_NUM_MAX/2) {
    // 从接收队列中移除队列头的数据包
    list_remove_head(&hciReadPktRxQueue, (tListNode **)&pckt);
    // 将移除的数据包插入到空闲数据包池的队列尾部
    list_insert_tail(&hciReadPktPool, (tListNode *)pckt);
  }
}

/********************** HCI Transport layer functions *****************************/

void hci_init(void(* UserEvtRx)(void* pData), void* pConf)
{
  uint8_t index;
  
  if(UserEvtRx != NULL)
  {
    hciContext.UserEvtRx = UserEvtRx; // 注册 HCI 回调函数
  }
  
  /* 初始化 BLE TRANSPORT 层
   * 就是初始化 SPI 初始化、反初始化、发送、接收、复位，获取时钟频率信息 回调函数传入 hciContext 接口体
   * 注册按键回调函数 --- hci_tl_lowlevel_isr
   */
  hci_tl_lowlevel_init();
  
  /* 初始化就绪和空闲hci数据包队列的列表头 */
  list_init_head(&hciReadPktPool);
  list_init_head(&hciReadPktRxQueue);
  
  /* 初始化空闲hci数据包队列，也就是说有 HCI_READ_PACKET_NUM_MAX(5) 个节点可以用来存储数据 */
  for (index = 0; index < HCI_READ_PACKET_NUM_MAX; index++)
  {
    list_insert_tail(&hciReadPktPool, (tListNode *)&hciReadPacketBuffer[index]);
  } 
  
  /* 初始化底层驱动 */
  if (hciContext.io.Init)  hciContext.io.Init(NULL); // 初始化 SPI
  if (hciContext.io.Reset) hciContext.io.Reset(); // 复位 SPI
}

// 注册 HCI 回调
void hci_register_io_bus(tHciIO* fops)
{
  /* Register bus function */
  hciContext.io.Init    = fops->Init; 
  hciContext.io.Receive = fops->Receive;  
  hciContext.io.Send    = fops->Send;
  hciContext.io.GetTick = fops->GetTick;
  hciContext.io.Reset   = fops->Reset;    
}

/**
  * @brief  发送 HCI 请求。
  *
  * @param  r      指向 HCI 请求结构的指针，包含 OGF、OCF、参数和期望的事件
  * @param  async  是否为异步请求（TRUE 表示异步，FALSE 表示同步）
  * @retval 0      成功
  * @retval -1     失败
  */
int hci_send_req(struct hci_request* r, BOOL async)
{
  uint8_t *ptr; // 指向事件数据的指针，用于遍历接收到的事件包数据
  // 将 OGF（操作组字段）和 OCF（操作字段）打包成操作码 Opcode，
  // 并将其从主机字节序转换为蓝牙协议所需的小端字节序
  uint16_t opcode = htobs(cmd_opcode_pack(r->ogf, r->ocf));
  hci_event_pckt *event_pckt; // HCI 事件包结构体指针，用于解析事件类型
  hci_uart_pckt *hci_hdr;     // HCI UART 包头结构体指针，用于获取包类型及数据偏移

  tHciDataPacket * hciReadPacket = NULL; // 用于存储从底层接收队列中取出的 HCI 数据包
  tListNode hciTempQueue; // 临时队列节点，用于暂存非当前请求相关的事件包
  
  // 初始化临时队列头，确保队列为空
  list_init_head(&hciTempQueue);

  // 释放之前可能残留在接收队列中的数据包，将它们归还给空闲池，防止资源耗尽
  free_event_list();
  
  // 发送 HCI 命令，参数包含 OGF、OCF、参数长度和参数内容
  send_cmd(r->ogf, r->ocf, r->clen, r->cparam);

  // 如果是异步请求，则不等待响应，直接返回成功
  if (async)
  {
    return 0;
  }

  // 同步请求模式：循环等待并处理事件，直到命令完成或出错
  while (1) 
  {
    evt_cmd_complete  *cc;
    evt_cmd_status    *cs;
    evt_le_meta_event *me; // EVT_LE_META_EVENT（低功耗子事件）结构体指针
    uint32_t len;          // 当前事件负载长度
    
    uint32_t tickstart = HAL_GetTick(); // 获取当前系统时钟，用于超时判断
      
    while (1)
    {
      // 如果传输时间超过 1s 那么表示传输超时
      if ((HAL_GetTick() - tickstart) > HCI_DEFAULT_TIMEOUT_MS)
      {
        goto failed;
      }
      // 如果有数据包到达，那么跳出循环
      if (!list_is_empty(&hciReadPktRxQueue)) 
      {
        break;
      }
    }
    
    /* 从 HCI 事件接收队列中取出一个数据包 */
    list_remove_head(&hciReadPktRxQueue, (tListNode **)&hciReadPacket);    
    
    // 数据包缓冲区起始地址即为 UART 包头
    hci_hdr = (void *)hciReadPacket->dataBuff;

    // 检查数据包类型是否为 HCI 事件包
    if (hci_hdr->type == HCI_EVENT_PKT)
    {
      event_pckt = (void *)(hci_hdr->data);
    
      // 计算指向事件数据部分的指针和长度
      ptr = hciReadPacket->dataBuff + (1 + HCI_EVENT_HDR_SIZE);
      len = hciReadPacket->data_len - (1 + HCI_EVENT_HDR_SIZE);
    
      switch (event_pckt->evt) 
      {      
      case EVT_CMD_STATUS: // HCI 命令状态事件
        cs = (void *) ptr;
        
        if (cs->opcode != opcode)
          goto failed;
        
        if (r->event != EVT_CMD_STATUS) {
          if (cs->status) {
            goto failed;
          }
          break;
        }

        r->rlen = MIN(len, r->rlen);
        BLUENRG_memcpy(r->rparam, ptr, r->rlen);
        goto done;
      
      case EVT_CMD_COMPLETE: // HCI 命令完成事件
        cc = (void *) ptr;
      
        if (cc->opcode != opcode)
          goto failed;
      
        ptr += EVT_CMD_COMPLETE_SIZE;
        len -= EVT_CMD_COMPLETE_SIZE;
      
        r->rlen = MIN(len, r->rlen);
        BLUENRG_memcpy(r->rparam, ptr, r->rlen);
        goto done;
      
      case EVT_LE_META_EVENT:
        me = (void *) ptr;
      
        if (me->subevent != r->event)
          break;
      
        len -= 1;
        r->rlen = MIN(len, r->rlen);
        BLUENRG_memcpy(r->rparam, me->data, r->rlen);
        goto done;
      
      case EVT_HARDWARE_ERROR:            
        goto failed;
      
      default:      
        break;
      }
    }
    
    /* If there are no more packets to be processed, be sure there is at list one
       packet in the pool to process the expected event.
       If no free packets are available, discard the processed event and insert it
       into the pool. */
    if (list_is_empty(&hciReadPktPool) && list_is_empty(&hciReadPktRxQueue)) {
      list_insert_tail(&hciReadPktPool, (tListNode *)hciReadPacket);
      hciReadPacket=NULL;
    }
    else {
      /* Insert the packet in a different queue. These packets will be
      inserted back in the main queue just before exiting from send_req(), so that
      these events can be processed by the application.
    */
    list_insert_tail(&hciTempQueue, (tListNode *)hciReadPacket);
      hciReadPacket=NULL;
    }
  }
  
failed: 
  if (hciReadPacket!=NULL) {
    list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket);
  }
  move_list(&hciReadPktRxQueue, &hciTempQueue);  
  return -1;
  
done:
  /* Insert the packet back into the pool.*/
  list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket); 
  move_list(&hciReadPktRxQueue, &hciTempQueue);
  return 0;
}

void hci_user_evt_proc(void)
{
  tHciDataPacket * hciReadPacket = NULL;
     
  /* process any pending events read */
  while (list_is_empty(&hciReadPktRxQueue) == FALSE)
  {
    list_remove_head (&hciReadPktRxQueue, (tListNode **)&hciReadPacket);
    if (hciContext.UserEvtRx != NULL)
    {
      hciContext.UserEvtRx(hciReadPacket->dataBuff);
    }
    list_insert_tail(&hciReadPktPool, (tListNode *)hciReadPacket);
  }
}

int32_t hci_notify_asynch_evt(void* pdata)
{
  tHciDataPacket * hciReadPacket = NULL;
  uint8_t data_len;
  
  int32_t ret = 0;
  
  if (list_is_empty (&hciReadPktPool) == FALSE)
  {
    /* Queuing a packet to read */
    list_remove_head (&hciReadPktPool, (tListNode **)&hciReadPacket);
    
    if (hciContext.io.Receive)
    {
      data_len = hciContext.io.Receive(hciReadPacket->dataBuff, HCI_READ_PACKET_SIZE);
      if (data_len > 0)
      {                    
        hciReadPacket->data_len = data_len;
        if (verify_packet(hciReadPacket) == 0)
          list_insert_tail(&hciReadPktRxQueue, (tListNode *)hciReadPacket);
        else
          list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket);          
      }
      else 
      {
        /* Insert the packet back into the pool*/
        list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacket);
      }
    }
  }
  else 
  {
    ret = 1;
  }
  return ret;

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
