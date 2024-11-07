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
char buffer_test[100];
char buffer_file_hex[1024 * 5];
char string_data[100];
uint8_t hex_data[30];
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
        char extracted_data[length+1];
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
        }
        strcat(buffer_file_hex, extracted_data);
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
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    printf("\n-char_to_byte fail \n");
    return STATUS_FAIL;
}
int format_file_hex(char *data)
{
	char *ptr = strtok(data,"\n");
	while(ptr!=NULL)
	{
		printf("\n-trlen%d\n",strlen(ptr));
		sprintf(string_data,ptr);
		check_format_intel_hex(string_data);
		printf("\n-string data %s\n",string_data);
		printf("\n-*pointer vi tri thu %s\n",ptr);
		ptr = strtok(NULL,"\n");
	    return STATUS_OK;
	}
    return STATUS_OK;
}
void convert_string_to_array_hex(char *input, uint8_t *output)
{
    uint8_t index = 0;
    while (input !=NULL && *input !='\r') {
        uint8_t hex_value = (char_to_byte(*input) << 4);
        input++;
        hex_value |= char_to_byte(*input);
        input++;
        output[index++] = hex_value;
        printf("\n%02X", hex_value);
    }
    printf("\n");
}
int check_format_intel_hex(char *data)
{
	if(*data==':')
	{
			data++;
			convert_string_to_array_hex(data,hex_data);
			if(hex_data[0]==2)
			{
				printf("\n-->byte count is %d\n",hex_data[0]);
				if(hex_data[3]==4)
				{
					printf("\n-->Record type is %d\n",hex_data[3]);
				}

			}
			printf("\n-check_format_intel_hex successfully ! \n");
	}
	else return STATUS_FAIL;
	return STATUS_OK;
}
int check_sum_intel_hex(char *data)
{

	return STATUS_OK;
}
