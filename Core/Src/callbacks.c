/*
 * callbacks.c
 *
 *  Created on: May 22, 2020
 *      Author: venkatadineshjakkampudi
 */

#include "callbacks.h"
#include "services.h"
#include "app_ble.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"

#include<stdbool.h>
#include<stdlib.h>

static volatile uint8_t CONNECTED = FALSE;
static volatile uint8_t NOTIFICATION_ENABLED = FALSE;

void cb_on_gap_connection_complete(uint8_t *, uint16_t);
void cb_on_gap_disconnection_complete(void);
void cb_on_read_request(uint16_t);
void cb_on_attribute_modified(uint16_t, uint16_t, uint8_t []);

uint8_t is_connected(void);


/*
 * @ brief return the status of connection
 * @retvalue True if connected, else False
 */
uint8_t is_connected(void){
	return CONNECTED;
}


/*
 * @ brief return the status of notifications
 */
uint8_t is_notification_enabled(void){
	return NOTIFICATION_ENABLED;
}

/*
 * @brief Call back called on successful connection complete
 * @param peer_addr An array that contains peer address
 * @param handle connection handle
 */
void cb_on_gap_connection_complete(uint8_t peer_addr[], uint16_t handle){
	CONNECTED = TRUE;
	set_connection_handle(handle);
	reset_connectable_status();
}


/*
 * @brief Call back called on successful disconnection
 * 			reset the active connection flags to make
 * 			BLE device to broadcast discovery packets
 * 			again
 */
void cb_on_gap_disconnection_complete(void){
	CONNECTED = FALSE;
	set_connection_handle(0);
	set_connectable_status();
}


/*
 * @brief 当设备请求读取特征值时调用的回调函数。
 *        该函数处理读取类型的特征值请求。
 * @param handle 特征句柄，用于标识具体的特征。
 */
void cb_on_read_request(uint16_t handle){
	// 检查是否是 LED 状态读取特征的句柄
	if(is_led_status_read_charac(handle))
		service_read_request(); // 调用服务层函数处理读取请求

	// 获取当前连接句柄
	uint16_t connection_handle = get_connection_handle();

	// 如果设备已连接且连接句柄有效
	if(CONNECTED && connection_handle!=0){
		aci_gatt_allow_read(connection_handle); // 允许客户端读取特征值
	}
}

/*
 * @brief 这是一个回调函数，当连接的设备修改属性时调用。
 *        例如，启用通知、从连接的设备写入数据等。
 * @param handle 特征句柄，用于与特定特征进行比较。
 * @param len 到达无线电的数据长度。
 * @param data 到达的数据。
 * @retvalue 无返回值。
 */
void cb_on_attribute_modified(uint16_t handle, uint16_t len, uint8_t data[]){
	// 检查是否是通知特征的句柄
	if(is_pb_notification_attribute(handle)){
		if(data[0] == 0x01) // 如果数据为 0x01，启用通知
			NOTIFICATION_ENABLED = TRUE;
		else // 如果数据为其他值，禁用通知
			NOTIFICATION_ENABLED = FALSE;

	}

	// 检查是否是 LED 控制特征的句柄
	if(is_led_control_attribute(handle)){
		change_led_state(len, data); // 根据接收到的数据更改 LED 状态
	}
}

/*
 * @brief This is call back called through interrupt on push button pressed
 * 			On PB pressed notify the client through notification characteristic
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	set_notification_pending();
}


