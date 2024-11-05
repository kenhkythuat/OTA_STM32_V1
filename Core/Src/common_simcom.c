/*
 * common_simcom.c
 *
 *  Created on: Oct 30, 2024
 *      Author: thuanphat7
 */
#include "common_simcom.h"
#include "config.h"
#include "main.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char rx_buffer[700];
char rx_data_sim[700];
int previousTick;
int timeout_pb_done = 40000;
bool is_connect_simcom = false;
bool is_pb_done = 0;
bool at_connect_mqtt = false;
bool is_at_acquier_mqtt = false;
bool is_at_subcribe_topic_mqtt = false;
bool is_at_subcribe_mqtt = false;
bool is_at_topic_puplish_mqtt = false;
bool is_at_data_puplish_mqtt = false;
bool is_at_puplish_mqtt = false;
bool is_at_check_dis_mqtt = false;
bool is_at_disconnect_mqtt = false;
bool is_at_rel_mqtt = false;
bool is_at_stop_mqtt = false;
bool is_inital_check = false;

bool is_enable_mqtt = false;
bool is_connected_mqtt = false;
bool is_checked_sim = false;
bool is_subcribed_mqtt = false;
bool is_published_mqtt = false;
bool is_acquiered_mqtt = false;
bool is_updated_status = false;

bool is_started_http = false;
enum GmsModemState current_status_simcom = Off;
float signal_strength = 0;
uint8_t total_errors = 0;
int rssi = -99;
uint16_t times_data = 0;

/* USER CODE END PTD */

void send_to_simcom_a76xx(char *cmd)
{
    printf("STM32 Write: %s", cmd);
    HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 500);
}

void enable_simcom(void)
{
    printf("Enable SIMCOM\r\n");
    HAL_GPIO_WritePin(A76XX_PWRKEY_GPIO_Port, A76XX_PWRKEY_Pin, SET);
    HAL_Delay(3000);
    HAL_GPIO_WritePin(A76XX_PWRKEY_GPIO_Port, A76XX_PWRKEY_Pin, RESET);
    HAL_Delay(3000);
    HAL_GPIO_WritePin(A76XX_PWRKEY_GPIO_Port, A76XX_PWRKEY_Pin, SET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(A76XX_PWRKEY_GPIO_Port, A76XX_PWRKEY_Pin, RESET);
    HAL_Delay(1000);
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    static int times;
    if (huart->Instance == USART1) {
        printf("\r\n-SIMCOM Response:");
        printf(rx_buffer);

        times = strlen(rx_buffer);
        for (int i = 0; i < times; i++) {
            rx_data_sim[i] = rx_buffer[i];
        }
        // HAL_DMA_Start(&hdma_memtomem_dma1_channel1,(uint32_t )&rx_buffer,(uint32_t )&rx_data_sim,times);
    }
    memset(rx_buffer, '\0', times);
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t *)rx_buffer, 700);
}

int wait_for_pb_done_event(void)
{
    previousTick = HAL_GetTick();
    while (previousTick + timeout_pb_done > HAL_GetTick()) {
        if (strstr((char *)rx_data_sim, "PB DONE")) {
            return 1;
        }
    }
    if (is_connect_simcom == 0) {
        NVIC_SystemReset();
    }

    return 0;
}

int read_signal_quality(void)
{
    send_to_simcom_a76xx("AT+CSQ\r\n");
    HAL_Delay(200);
    signal_strength = (rx_data_sim[8] - 48) * 10 + (rx_data_sim[9] - 48);
    if (signal_strength >= 31) {
        rssi = -51;
    } else if (signal_strength <= 0) {
        rssi = -113;
    } else
        rssi = (signal_strength * 2 - 113);
    return rssi;
}

float check_signal_simcom(void)
{
    printf("-----------------check_signal_simcom------------------\n");
    send_to_simcom_a76xx("ATE0\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CSQ\r\n");
    HAL_Delay(200);
    signal_strength = (rx_data_sim[8] - 48) * 10 + (rx_data_sim[9] - 48);
    if (signal_strength >= 31) {
        rssi = -51;
    }
    if (signal_strength >= 99) {
        NVIC_SystemReset();
    } else {
        rssi = (signal_strength * 2 - 113);
    }
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CPIN?\r\n");
    HAL_Delay(200);
    if (strstr((char *)rx_data_sim, "+CPIN: READY")) {
        printf("-----------------SIM OK !------------------\n");
    } else
        return 0;
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CREG=0\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CREG?\r\n");
    HAL_Delay(200);
    if (strstr((char *)rx_data_sim, "+CREG: 0,1") || strstr((char *)rx_data_sim, "+CREG: 0,6")) {
        printf("-----------------Network registration OK!------------------\n");
    } else {
        printf("-----------------Network registration fail!------------------\n");
        return 0;
    }
    send_to_simcom_a76xx("AT+CNMP=2\r\n");
    HAL_Delay(1000);
    send_to_simcom_a76xx("AT+CNMP?\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CGATT=1\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CIICR\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CIFSR\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CGDCONT?\r\n");
    HAL_Delay(200);

    send_to_simcom_a76xx("AT+CPSI?\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+GMR\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+COPS?\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("ATI\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CICCID\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+CGREG?\r\n");
    HAL_Delay(200);

    if (strstr((char *)rx_data_sim, "+CGREG: 0,1")) {
        printf("-----------------Network registration OK!------------------\n");
    } else
        return 0;
    return 1;
}

void restart_stm32(void)
{
    printf("\r\n-----------------Restart STM32------------------\r\n");
    printf("\r\n-----------------GOOD BYE !------------------\r\n");
    // stop_mqtt_via_gsm();
    NVIC_SystemReset();
}
int start_http_service(void)
{
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPINIT=?\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPINIT\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/kenhkythuat/ota_stm32_v2/master/Debug/blink_led_ota_v1.hex\"\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPPARA=\"CONNECTTO\",60\r\n");
    HAL_Delay(300);
    send_to_simcom_a76xx("AT+HTTPPARA=\"RECVTO\",60\r\n");
    HAL_Delay(300);
    send_to_simcom_a76xx("AT+HTTPACTION=0\r\n");
    HAL_Delay(3000);
    send_to_simcom_a76xx("AT+HTTPREAD=0,256\r\n");
    HAL_Delay(2000);
    send_to_simcom_a76xx("AT+HTTPREAD=0,256\r\n");
    HAL_Delay(2000);

    if (strstr((char *)rx_data_sim, "+HTTPREAD: 0")) {
        printf("-----------------Start HTTP service successfully------------------\n");
        return 1;
    }
    return 0;
}
int get_hhtp_service(void)
{
    return 1;
}
bool get_file_hex_form_simcom(void)
{
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPINIT=?\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPINIT\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPPARA=\"URL\",\"https://raw.githubusercontent.com/kenhkythuat/ota_stm32_v2/master/Debug/blink_led_ota_v1.hex\"\r\n");
    HAL_Delay(200);
    send_to_simcom_a76xx("AT+HTTPPARA=\"CONNECTTO\",60\r\n");
    HAL_Delay(300);
    send_to_simcom_a76xx("AT+HTTPPARA=\"RECVTO\",60\r\n");
    HAL_Delay(300);
    memset(rx_data_sim, '\0', 700);
    send_to_simcom_a76xx("AT+HTTPACTION=0\r\n");
    HAL_Delay(7000);
    times_data = (get_length_data_form_file_hex(rx_data_sim) / 100 + 1);
    printf("-number of times data is %d bytes \n", times_data);
    for (int i = 0; i <100; i++) {
        printf("\r\n-----------------HTTPACTION SUSSES------------------\n");
        send_to_simcom_a76xx("AT+HTTPREAD=0,100\r\n");
        HAL_Delay(1000);
        cut_data_form_simcom(rx_data_sim);
    }
    printf("sau khi ghep chuoi \r\n%s", buffer_file_hex);
    //	  if(strstr((char *)rx_data_sim, "+HTTPACTION: 0,200"))
    //	  {
    //		  printf("-----------------HTTPACTION SUSSES------------------\n");
    //			send_to_simcom_a76xx("AT+HTTPREAD=0,100\r\n");
    //			HAL_Delay(2000);
    //			cut_data_form_simcom(rx_data_sim);
    //			send_to_simcom_a76xx("AT+HTTPREAD=0,100\r\n");
    //			HAL_Delay(2000);
    //			cut_data_form_simcom(rx_data_sim);
    //			printf("sau khi ghep chuoi \r\n%s",buffer_file_hex);
    //
    //	  }
    if (strstr((char *)rx_data_sim, "+HTTPREAD: 100")) {
        printf("-----------------READ 100 BYTES DATA SUSSES------------------\n");
    }
    HAL_Delay(2000);
    return STATUS_OK;
}
