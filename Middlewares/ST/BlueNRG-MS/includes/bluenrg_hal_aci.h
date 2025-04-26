/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
* File Name          : bluenrg_hal_aci.h
* Author             : AMS - AAS
* Version            : V1.0.0
* Date               : 26-Jun-2014
* Description        : Header file with HCI commands for BlueNRG
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __BLUENRG_HAL_ACI_H__
#define __BLUENRG_HAL_ACI_H__

/** 
 * @addtogroup HIGH_LEVEL_INTERFACE HIGH_LEVEL_INTERFACE
 * @{
 */
 
/**
 *@addtogroup ACI_HAL ACI_HAL
 *@brief API for HAL layer.
 *@{
 */

/**
 * @defgroup HAL_Functions HAL functions
 * @brief API for BlueNRG HAL layer.
 * @{
 */

/**
 * @brief 获取固件的构建号。
 * @param[out] build_number 返回标识固件发布版本的构建号。
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_get_fw_build_number(uint16_t *build_number);

/**
 * @brief 将一个值写入低级配置数据结构。
 * @note  该命令可在运行时直接设置系统的某些底层参数。
 * @param offset 数据结构中的偏移量。数据结构的第一个成员偏移为 0。\n
 *               详见 @ref Config_vals。
 * @param len    要写入的数据长度
 * @param[out] val 要写入的数据缓冲区指针
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_write_config_data(uint8_t offset, 
                                    uint8_t len,
                                    const uint8_t *val);
/**
 * @brief 从低级配置数据结构中读取值。
 *        不同的偏移量可导致读取字节数的变化。
 * @param offset           数据结构中的偏移量。数据结构的第一个成员偏移为 0。\n
 *                         详见 @ref Config_vals。
 * @param data_len         数据缓冲区的长度
 * @param[out] data_len_out_p  读取到的数据长度输出指针
 * @param[out] data        存放读取数据的缓冲区指针
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_read_config_data(uint8_t offset, uint16_t data_len, uint8_t *data_len_out_p, uint8_t *data);

/**
 * @brief 设置 BlueNRG 的发射功率级别。
 * @note  通过控制 `en_high_power` 和 `pa_level` 的组合来确定输出功率（dBm）。
 *        系统启动或重启后默认使用最大 8 dBm 的功率级别。调用此命令后，
 *        输出功率会立即生效，无论是否正在进行蓝牙通信。可用于调试时
 *        持续广播并观察信号强度变化。此设置会一直保留到下一次调用
 *        设置发射功率命令或系统重启为止。
 * @param en_high_power   只可为 0 或 1，设置高功率位的开关。请根据所选 RF 硬件
 *                        配置（普通模式或高功率模式）合理设置。
 * @param pa_level        PA 级别，可取值 0～7。
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_set_tx_power_level(uint8_t en_high_power, uint8_t pa_level);

/**
 * @brief 返回直接测试模式下发送的数据包数量。
 * @note  当直接发送测试（Direct TX Test）开始时，会启用一个 32 位计数器来统计已发送的数据包数量。
 *        该命令可用于查询在直接发送测试期间已发送的数据包总数。\n
 *        计数器从 0 开始累加，若达到最大值后会重绕（wrap）并重新从 0 开始计数。
 *        计数器仅在下一次直接发送测试开始时被清零。
 * @param[out] number_of_packets 指向存放上次直接发送测试发送数据包数量的变量
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_le_tx_test_packet_number(uint32_t *number_of_packets);

/**
 * @brief 让设备进入待机（Standby）模式。
 * @note  正常情况下，BlueNRG 会自动进入睡眠（Sleep）模式以节省功耗。此命令将设备置于待机模式，
 *        而非睡眠模式。两者区别在于：在睡眠模式下，设备可通过内部定时器自动唤醒；而在待机模式下，
 *        内部定时器被禁用，仅能通过外部信号（例如通过 SPI 总线发送的 HCI 命令）唤醒设备。\n
 *        仅当设备无其它蓝牙活动时，该命令才会被接受；否则会返回错误码 `ERR_COMMAND_DISALLOWED`。
 *
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_device_standby(void);

/**
 * @brief 在指定信道上发射载波（音频）信号。
 * @note  在特定信道上发射正弦波信号，仅供测试用途。
 *        信道编号取值范围为 0 ～ 39，共 40 个 BLE 信道：例如信道 0 对应 2.402 GHz，中频减 250 kHz 后发射；
 *        信道 1 对应 2.404 GHz，以此类推。\n
 *        正常蓝牙活动进行时，请勿使用此命令。测试完成后，应调用 `aci_hal_tone_stop()` 停止音频信号。
 *
 * @param rf_channel BLE 信道编号，取值 0 ～ 39（物理发射频率为 2.402 + 0.002×rf_channel GHz，再减 250 kHz）
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_tone_start(uint8_t rf_channel);

/**
 * @brief 停止先前由 aci_hal_tone_start() 启动的音频信号发射命令。
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_tone_stop(void);

/**
 * @brief 返回所有连接的状态信息。
 * @note  该命令返回设备所管理的 8 条低功耗蓝牙链路的状态。
 * @param[out] link_status   存放每条链路状态的数组（长度 8）。详见 @ref Link_Status。
 * @param[out] conn_handle   存放每条链路连接句柄的数组（长度 8）。
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_get_link_status(uint8_t link_status[8], uint16_t conn_handle[8]);

/**
 * @brief 返回当前的锚点周期及最大可用时隙长度。
 * @note  该命令提供锚点周期的信息，帮助应用在多链路场景中选择合适的时隙时序。
 * @param[out] anchor_period  当前锚点周期（以 0.625 ms 为单位的倍数）。
 * @param[out] max_free_slot  最大可用时隙长度（以 0.625 ms 为单位的倍数）。
 * @return 返回表示成功或错误代码的值。
 */
tBleStatus aci_hal_get_anchor_period(uint32_t *anchor_period, uint32_t *max_free_slot);

/**
 * @}
 */

/**
 * @defgroup HAL_Events HAL 事件
 * 这些结构体是 @ref evt_blue_aci 的数据字段。
 * @{
 */

/** 
 * HCI 特定厂商事件，在 BlueNRG 上电或重启时触发。 
 */
#define EVT_BLUE_HAL_INITIALIZED                 (0x0001)
typedef __packed struct _evt_hal_initialized{
  uint8_t reason_code; /**< 重置原因，详见 @ref Reset_Reasons */
} PACKED evt_hal_initialized;

/**
 * 当外部微控制器读取的事件队列发生溢出时，产生此事件。
 * 通常因为外部微控制器未及时读取待处理事件而导致。
 * 返回的位图指示哪些事件被丢失。每个位被设置为 1 表示对应事件发生过一次或多次。
 * 事件 EVT_BLUE_HAL_EVENTS_LOST 不会被丢失，只要队列有空位，就会立即插入该事件。
 * 在外部微控制器通过 IRQ 引脚及时读取事件的正常情况下，不应出现此事件。
 * 此事件用于检测外部微控制器的异常行为，或在关键事件丢失时帮助应用恢复。
 */
#define EVT_BLUE_HAL_EVENTS_LOST_IDB05A1                (0x0002)
typedef __packed struct _evt_hal_events_lost{
  uint8_t  lost_events[8]; /**< 丢失事件的位图，每个位表示对应事件发生过一次或多次，详见 @ref Lost_Events */
} PACKED evt_hal_events_lost_IDB05A1;


/**
 * @brief 在检测到系统崩溃时，向应用程序报告崩溃信息。
 * @note  本事件在 @ref ACI_BLUE_INITIALIZED_EVENT 之后触发，用于调试。返回的各种寄存器与类型信息
 *        有助于分析崩溃根本原因。
 */
#define EVT_BLUE_HAL_CRASH_INFO_IDB05A1                 (0x0003)
typedef __packed struct _evt_hal_crash_info{
  uint8_t  crash_type;      /**< 崩溃类型：断言失败（0）、NMI 异常（1）、硬件故障（2） */
  uint32_t sp;              /**< SP 寄存器 */
  uint32_t r0;              /**< R0 寄存器 */
  uint32_t r1;              /**< R1 寄存器 */
  uint32_t r2;              /**< R2 寄存器 */
  uint32_t r3;              /**< R3 寄存器 */
  uint32_t r12;             /**< R12 寄存器 */
  uint32_t lr;              /**< LR 寄存器 */
  uint32_t pc;              /**< PC 寄存器 */
  uint32_t xpsr;            /**< xPSR 寄存器 */
  uint8_t  debug_data_len;  /**< debug_data 字段长度 */
  uint8_t  debug_data[];    /**< 调试数据 */
} PACKED evt_hal_crash_info_IDB05A1;


/**
 * @}
 */


/**
 * @anchor Reset_Reasons
 * @name 重置原因
 * 详见 @ref EVT_BLUE_HAL_INITIALIZED。
 * @{
 */
#define RESET_NORMAL            1 /**< 正常启动。 */
#define RESET_UPDATER_ACI       2 /**< 通过 ACI 命令进入升级模式 */
#define RESET_UPDATER_BAD_FLAG  3 /**< 由于无效的 BLUE 标志进入升级模式 */
#define RESET_UPDATER_PIN       4 /**< 通过 IRQ 引脚进入升级模式 */
#define RESET_WATCHDOG          5 /**< 由看门狗复位导致的重置 */
#define RESET_LOCKUP            6 /**< 由于锁死（Lockup）导致的重置 */
#define RESET_BROWNOUT          7 /**< 棕断（Brownout）复位 */
#define RESET_CRASH             8 /**< 由于系统崩溃（NMI 或 Hard Fault）导致的重置 */
#define RESET_ECC_ERR           9 /**< 由于 ECC 错误导致的重置 */
/**
 * @}
 */


/**
 * @defgroup Config_vals 配置值的偏移量和长度
 * @brief 配置值的偏移量和长度说明。
 *        详见 aci_hal_write_config_data()。
 * @{
 */

/**
 * @name 配置值列表
 * 详见 @ref aci_hal_write_config_data()。
 * @{
 */
#define CONFIG_DATA_PUBADDR_OFFSET          (0x00) /**< 蓝牙公共地址 */
#define CONFIG_DATA_DIV_OFFSET              (0x06) /**< 用于派生 CSRK 的 DIV 值 */
#define CONFIG_DATA_ER_OFFSET               (0x08) /**< 用于派生 LTK 和 CSRK 的加密根密钥 */
#define CONFIG_DATA_IR_OFFSET               (0x18) /**< 用于派生 LTK 和 CSRK 的身份根密钥 */
#define CONFIG_DATA_LL_WITHOUT_HOST         (0x2C) /**< 开启/关闭仅链路层模式。设置为 1 以禁用主机
 	 	 	 	 	 	 	 	 	 	 	 	 	                              仅当 aci_hal_write_config_data() 是复位后执行的第一个命令时才可写入。 */
#define CONFIG_DATA_RANDOM_ADDRESS          (0x80) /**< 存储的静态随机地址。只读 */

/**
 * @brief 选择 BlueNRG 模式配置
 *
 * @li 模式 1：从机或主机，1 个连接，仅使用 RAM1（小型 GATT 数据库）
 * @li 模式 2：从机或主机，1 个连接，使用 RAM1 和 RAM2（大型 GATT 数据库）
 * @li 模式 3：主机/从机，8 个连接，使用 RAM1 和 RAM2
 * @li 模式 4：主机/从机，4 个连接，同时进行扫描和广播，使用 RAM1 和 RAM2
 */
#define CONFIG_DATA_MODE_OFFSET 			(0x2D)

/** 
 * @brief 设置为 1 可禁用看门狗，默认启用 
 */
#define CONFIG_DATA_WATCHDOG_DISABLE 		(0x2F)

/**
 * @}
 */

/**
 * @name 配置值的长度
 * @brief 详见 @ref aci_hal_write_config_data()
 * @{
 */
/**
 * @brief 配置值的长度定义
 * @details 这些宏定义了在调用 `aci_hal_write_config_data` 或 `aci_hal_read_config_data` 时，
 *          每个配置项的长度（以字节为单位）。
 */
#define CONFIG_DATA_PUBADDR_LEN             (6)  /**< 蓝牙公共地址的长度（6 字节）。 */
#define CONFIG_DATA_DIV_LEN                 (2)  /**< 用于派生 CSRK 的 DIV 值长度（2 字节）。 */
#define CONFIG_DATA_ER_LEN                  (16) /**< 加密根密钥（Encryption Root Key）的长度（16 字节）。 */
#define CONFIG_DATA_IR_LEN                  (16) /**< 身份根密钥（Identity Root Key）的长度（16 字节）。 */
#define CONFIG_DATA_LL_WITHOUT_HOST_LEN     (1)  /**< 链路层模式配置的长度（1 字节）。 */
#define CONFIG_DATA_MODE_LEN                (1)  /**< 模式配置的长度（1 字节）。 */
#define CONFIG_DATA_WATCHDOG_DISABLE_LEN    (1)  /**< 看门狗禁用配置的长度（1 字节）。 */

/**
 * @}
 */

/**
 * @anchor Link_Status
 * @name 链路状态
 * 详见 @ref aci_hal_get_link_status()。
 * @{
 */
#define STATUS_IDLE                         0 /**< 空闲状态，设备未执行任何操作。 */
#define STATUS_ADVERTISING                  1 /**< 广播状态，设备正在发送广播包。 */
#define STATUS_CONNECTED_AS_SLAVE           2 /**< 从机连接状态，设备作为从机已建立连接。 */
#define STATUS_SCANNING                     3 /**< 扫描状态，设备正在扫描其他设备的广播包。 */
#define STATUS_CONNECTED_AS_MASTER          5 /**< 主机连接状态，设备作为主机已建立连接。 */
#define STATUS_TX_TEST                      6 /**< 发送测试状态，设备正在执行发射测试。 */
#define STATUS_RX_TEST                      7 /**< 接收测试状态，设备正在执行接收测试。 */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @anchor Lost_Events
 * @name 丢失事件位图
 * 详见 @ref EVT_BLUE_HAL_EVENTS_LOST
 * @{
 */  
#define EVT_DISCONN_COMPLETE_BIT                                     0
#define EVT_ENCRYPT_CHANGE_BIT                                       1
#define EVT_READ_REMOTE_VERSION_COMPLETE_BIT                         2
#define EVT_CMD_COMPLETE_BIT                                         3
#define EVT_CMD_STATUS_BIT                                           4
#define EVT_HARDWARE_ERROR_BIT                                       5
#define EVT_NUM_COMP_PKTS_BIT                                        6
#define EVT_ENCRYPTION_KEY_REFRESH_BIT                               7
#define EVT_BLUE_HAL_INITIALIZED_BIT                                 8
#define EVT_BLUE_GAP_SET_LIMITED_DISCOVERABLE_BIT                    9
#define EVT_BLUE_GAP_PAIRING_CMPLT_BIT                               10
#define EVT_BLUE_GAP_PASS_KEY_REQUEST_BIT                            11
#define EVT_BLUE_GAP_AUTHORIZATION_REQUEST_BIT                       12
#define EVT_BLUE_GAP_SECURITY_REQ_INITIATED_BIT                      13
#define EVT_BLUE_GAP_BOND_LOST_BIT                                   14
#define EVT_BLUE_GAP_PROCEDURE_COMPLETE_BIT                          15
#define EVT_BLUE_GAP_ADDR_NOT_RESOLVED_BIT                           16
#define EVT_BLUE_L2CAP_CONN_UPDATE_RESP_BIT                          17
#define EVT_BLUE_L2CAP_PROCEDURE_TIMEOUT_BIT                         18
#define EVT_BLUE_L2CAP_CONN_UPDATE_REQ_BIT                           19
#define EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BIT                         20
#define EVT_BLUE_GATT_PROCEDURE_TIMEOUT_BIT                          21
#define EVT_BLUE_EXCHANGE_MTU_RESP_BIT                               22
#define EVT_BLUE_ATT_FIND_INFORMATION_RESP_BIT                       23
#define EVT_BLUE_ATT_FIND_BY_TYPE_VAL_RESP_BIT                       24
#define EVT_BLUE_ATT_READ_BY_TYPE_RESP_BIT                           25
#define EVT_BLUE_ATT_READ_RESP_BIT                                   26
#define EVT_BLUE_ATT_READ_BLOB_RESP_BIT                              27
#define EVT_BLUE_ATT_READ_MULTIPLE_RESP_BIT                          28
#define EVT_BLUE_ATT_READ_BY_GROUP_RESP_BIT                          29
#define EVT_BLUE_ATT_WRITE_RESP_BIT                                  30
#define EVT_BLUE_ATT_PREPARE_WRITE_RESP_BIT                          31
#define EVT_BLUE_ATT_EXEC_WRITE_RESP_BIT                             32
#define EVT_BLUE_GATT_INDICATION_BIT                                 33
#define EVT_BLUE_GATT_NOTIFICATION_BIT                               34
#define EVT_BLUE_GATT_PROCEDURE_COMPLETE_BIT                         35
#define EVT_BLUE_GATT_ERROR_RESP_BIT                                 36
#define EVT_BLUE_GATT_DISC_READ_CHARAC_BY_UUID_RESP_BIT              37
#define EVT_BLUE_GATT_WRITE_PERMIT_REQ_BIT                           38
#define EVT_BLUE_GATT_READ_PERMIT_REQ_BIT                            39
#define EVT_BLUE_GATT_READ_MULTI_PERMIT_REQ_BIT                      40
#define EVT_BLUE_GATT_TX_POOL_AVAILABLE_BIT                          41
#define EVT_BLUE_GATT_SERVER_RX_CONFIRMATION_BIT                     42
#define EVT_BLUE_GATT_PREPARE_WRITE_PERMIT_REQ_BIT                   43
#define EVT_LL_CONNECTION_COMPLETE_BIT                               44
#define EVT_LL_ADVERTISING_REPORT_BIT                                45
#define EVT_LL_CONNECTION_UPDATE_COMPLETE_BIT                        46
#define EVT_LL_READ_REMOTE_USED_FEATURES_BIT                         47
#define EVT_LL_LTK_REQUEST_BIT                                       48   
/**
 * @}
 */

/**
 * @name 硬件错误事件代码
 * 详见 @ref EVT_HARDWARE_ERROR。
 * @{
 */
/**
 * 检测到 SPI 总线错误，通常由于外部微控制器的 SPI 配置不正确引起。
 */
#define SPI_FRAMING_ERROR           0
/**
 * 由于晶振启动缓慢引起，表明设备配置中的 HS_STARTUP_TIME 需要调整。
 * 出现此事件后，建议对设备进行硬件重置。
 */
#define RADIO_STATE_ERROR           1
/**
 * 由于晶振启动缓慢引起，表明设备配置中的 HS_STARTUP_TIME 需要调整。
 * 出现此事件后，建议对设备进行硬件重置。
 */
#define TIMER_OVERRUN_ERROR         2 
/**
 * @}
 */
 
/**
 * @}
 */

/**
 * @}
 */

#endif /* __BLUENRG_HAL_ACI_H__ */
