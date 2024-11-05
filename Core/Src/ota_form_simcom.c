/*
 * ota_form_simcom.c
 *
 *  Created on: Nov 4, 2024
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

char buffer_file_hex[1024*20];
void cut_data_form_simcom(char *data)
{
    char *start = strstr(data, "+HTTPREAD: 100\r\n");
    if (start != NULL) {
        start += strlen("+HTTPREAD: 100\r\n");
    }
    char *end = strstr(start, "+HTTPREAD: 0");
    if (start != NULL && end != NULL) {
        int length = end - start;
        printf("do dai chuoi la %d\r\n", length);
        char extracted_data[length + 1];
        strncpy(extracted_data, start, length);
        if (extracted_data[0] == 13) {
            printf("phia truoc co xuong dong!!\r\n");
            start = start + 2;
            length = end - start;
        }
        strncpy(extracted_data, start, length);
        if (extracted_data[length - 45] == 58) {
            printf("ghep chuoi la dung\r\n");
            extracted_data[length] = '\0';
        } else {
            printf("ghep chuoi la sai !!!!\r\n");
            extracted_data[length - 1] = '\0';
            extracted_data[length - 2] = '\0';
            extracted_data[length] = '\0';
        }
        strcat(buffer_file_hex, extracted_data);
        // printf("chuoi da cat:\n%s\n", extracted_data);
    } else {
        printf("khong cat duoc chuoi\n");
    }
    memset(rx_data_sim, '\0', 700);
}
int get_length_data_form_file_hex(char *data_file_hex)
{
    char *token_size;
    int number = 0;
    token_size = strtok(data_file_hex, ",");
    while (token_size != NULL) {
        number = atoi(token_size);
        token_size = strtok(NULL, ",");
    }
    printf("-Hex file length is %d bytes \n", number);
    return number;
}
int char_to_byte(char c)
{
	if(c>='0'&&c<='9') return c-'0';
	if(c>='A'&&c<='F') return c-'A'+10;
	if(c>='a'&&c<='f') return c-'a'+10;
	return STATUS_OK;
}
