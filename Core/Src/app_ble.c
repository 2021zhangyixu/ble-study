#include "app_ble.h"
#include "bluenrg_gap.h"
#include "hci_const.h"
#include "bluenrg_hal_aci.h"
#include "hci.h"
#include "hci_le.h"
#include "bluenrg_gatt_aci.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_utils.h"
#include "services.h"
#include "callbacks.h"

#include <stdint.h>
#include <stdbool.h>

#define BDADDR_SIZE 6

static bool CONNECTABLE = TRUE;

void MX_BlueNRG_MS_Init(void);
void MX_BlueNRG_MS_Process(void);
void event_user_notify(void *);

/* @brief BlueNRG-MS initialization
 * @retvalue None
 *
 */
void MX_BlueNRG_MS_Init(void){
	tBleStatus ret;
	const char* name = "Jakkampudi"; // 蓝牙设备名称
	uint8_t server_bdaddr[] = {0x01,0x02,0x03,0x04,0x05,0x06}; // 蓝牙设备的服务器地址
	uint8_t bdaddr[BDADDR_SIZE]; // 存储蓝牙地址的缓冲区
	uint16_t service_handle, dev_name_char_handle, appearance_char_handle; // 服务句柄、设备名称句柄和外观句柄

	/* 初始化 HCI（Host Controller Interface）
	 * 注册 BLE 回调函数 --- event_user_notify
	 * 注册按键回调函数  --- hci_tl_lowlevel_isr
	 */
	hci_init(&event_user_notify, NULL);
	hci_reset();    // 复位 control 芯片
	HAL_Delay(100); // 延迟 100 毫秒，确保 control 复位完成

	// 将服务器地址复制到本地地址缓冲区
	BLUENRG_memcpy(bdaddr, server_bdaddr, sizeof(server_bdaddr));

	// 配置蓝牙设备的公共地址
	aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
			CONFIG_DATA_PUBADDR_LEN,
			bdaddr);

	// 初始化 GATT（Generic Attribute Profile）
	aci_gatt_init();
	// 初始化 GAP（Generic Access Profile），设置设备角色为外设，并初始化服务和特征句柄
	aci_gap_init_IDB05A1(GAP_PERIPHERAL_ROLE_IDB05A1, 0, strlen(name), &service_handle, &dev_name_char_handle, &appearance_char_handle);
	// 更新设备名称特征值
	aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
			strlen(name), (uint8_t *)name);

	// 初始化自定义服务
	addNucleoService(); // 添加 Nucleo 服务
	addPbService(); // 添加按键服务
}

/*
 * @brief BlueNRG-MS 后台任务处理函数
 * @retvalue 无
 */
void MX_BlueNRG_MS_Process(void){

    // 如果设备处于可连接状态，则尝试建立连接
	if(CONNECTABLE == TRUE)
		establish_connection(); // 调用函数以通过广播建立连接

	send_notification(); // 发送通知数据（如果有需要）
	hci_user_evt_proc(); // 处理 HCI 用户事件
}

/*
 * @brief set connectable status on disconnection complete
 */
void set_connectable_status(void){
	CONNECTABLE = TRUE;
}

/*
 * @brief reset connectable status on connection complete
 */
void reset_connectable_status(void){
	CONNECTABLE = FALSE;
}

/*
 * 通过可发现模式的间接广告建立联系
 */
tBleStatus establish_connection(void){
	tBleStatus ret;
	// 定义本地设备名称，包含完整的设备名称字段（AD_TYPE_COMPLETE_LOCAL_NAME）
	const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, 'D', 'i', 'n', 'e', 's', 'h', '-', 'L', 'a', 'b'};

    // 设置扫描响应数据为空（0 字节）
	hci_le_set_scan_resp_data(0, NULL);

	// 设置设备为可发现模式（Discoverable Mode），并通过广播发送设备名称
    ret = aci_gap_set_discoverable(
		ADV_IND,              // 广播类型：间接广播
		0,                    // 最小广告间隔（单位：625微秒）
		0,                    // 最大广告间隔（单位：625微秒）
		PUBLIC_ADDR,          // 使用公共地址
		NO_WHITE_LIST_USE,    // 不使用白名单
		sizeof(local_name),   // 广播数据的长度
		local_name,           // 广播数据（设备名称）
		0,                    // 无服务 UUID 列表
		NULL,                 // 服务 UUID 列表为空
		0,                    // 无服务 UUID 列表长度
		0                     // 无服务 UUID 列表长度
    );
	return ret;
}

/*
 * @brief 这是一个状态机，根据到达 HCI（Host Controller Interface）的数据，将数据转发到适当的回调函数。
 * @param *pData 指向到达 HCI 的数据缓冲区的指针。
 */
void event_user_notify(void *pData)
{
	// 将传入的数据指针转换为 HCI UART 数据包结构
	hci_uart_pckt *hci_pkt = pData;
	// 提取 HCI 事件数据包
	hci_event_pckt *hci_evt_pkt = (void *)hci_pkt->data;

	// 如果数据包类型不是 HCI 事件包，则直接返回
	if(hci_pkt->type != HCI_EVENT_PKT)
		return;

	// 根据事件类型进行处理
	switch(hci_evt_pkt->evt){
		case EVT_DISCONN_COMPLETE: // 断连事件
		{
			// 调用 GAP 层断连完成的回调函数
			cb_on_gap_disconnection_complete();
		}
		break;
		case  EVT_LE_META_EVENT:
		{
			evt_le_meta_event *hci_meta_evt = (void *)hci_evt_pkt->data;
			switch(hci_meta_evt->subevent) {
				case EVT_LE_CONN_COMPLETE: // LE 连接完成事件
				{
					// 提取 LE 连接完成事件数据
					evt_le_connection_complete *hci_con_comp_evt = (void *)hci_meta_evt->data;
					// 调用 GAP 层连接完成的回调函数，传入对端地址和连接句柄
					cb_on_gap_connection_complete(hci_con_comp_evt->peer_bdaddr, hci_con_comp_evt->handle);
				}
				break;
			}
		}
		break;
		case EVT_VENDOR: // 厂商自定义事件
		{
			// 提取厂商事件数据
			evt_blue_aci *vendor_evt = (void *)hci_evt_pkt->data;
			// 根据厂商事件代码进行处理
			switch(vendor_evt->ecode){
				case EVT_BLUE_GATT_READ_PERMIT_REQ: // GATT 读许可请求事件
				{
					// 提取读许可请求事件数据
					evt_gatt_read_permit_req *read_pmt_req_evt = (void *)vendor_evt->data;
					// 调用读请求的回调函数，传入属性句柄
					cb_on_read_request(read_pmt_req_evt->attr_handle);
				}
				break;
				case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED: // GATT 属性修改事件
				{
					// 提取属性修改事件数据
					evt_gatt_attr_modified_IDB05A1 *attr_modified_evt = (void *)vendor_evt->data;
					// 调用属性修改的回调函数，传入属性句柄、数据长度和修改后的数据，这里最终会改变开发板上绿灯的亮灭
					cb_on_attribute_modified(attr_modified_evt->attr_handle,
							attr_modified_evt->data_length,
							attr_modified_evt->att_data);

				}
				break;
			}
		}
		break;
	}
}
