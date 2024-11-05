/*
 * common_simcom.h
 *
 *  Created on: Oct 30, 2024
 *      Author: thuanphat7
 */


#ifndef INC_COMMON_SIMCOM_H_
#define INC_COMMON_SIMCOM_H_

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "config.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
/* USER CODE END Includes */


/* USER CODE BEGIN EFP */
extern void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size);
extern DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
extern UART_HandleTypeDef huart1;
extern void enable_simcom(void);
extern void send_to_simcom_a76xx(char *cmd);
extern int wait_for_pb_done_event(void);
extern void restart_stm32(void);
extern float check_signal_simcom(void);
extern int start_http_service(void);
extern bool get_file_hex_form_simcom(void);
extern void cut_data_form_simcom(char * data);
extern int get_length_data_form_file_hex(char * data_file_hex);
/* USER CODE END EFP */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
enum GmsModemState {
  Off,
  On,
  Internet_ready,
  Http_ready,
  Mqtt_ready,
  Subscribed_MQTT,
  Update_to_server,
  Ota_form_simcom
};
enum status {
	STATUS_OK,
	STATUS_FAIL
};
extern enum GmsModemState current_status_simcom;
extern bool is_connect_simcom;
extern bool is_pb_done;
extern bool is_enable_mqtt;
extern bool is_connected_mqtt;
extern bool is_checked_sim;
extern bool is_subcribed_mqtt;
extern bool is_published_mqtt;
extern bool is_acquiered_mqtt;
extern bool is_updated_status;
extern char rx_buffer[700];
extern char rx_data_sim[700];
extern float signal_strength;
extern uint8_t total_errors;
extern int rssi;
extern bool is_started_http;
extern char buffer_file_hex[1024*20];

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */



#endif /* INC_COMMON_SIMCOM_H_ */
