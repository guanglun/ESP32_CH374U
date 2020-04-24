
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "log.h"

#include "esp_log.h"



void printf_byte(uint8_t *buf,uint16_t len)
{
	uint8_t buffer_tmp[32];
	uint8_t buffer_log[1024];
	uint16_t count = 0;
	buffer_log[0] = '\0';

	for(;count < len;count++)
	{
		
		sprintf((char *)buffer_tmp, "%02X ",*(buf + count));
		strcat((char *)buffer_log,(const char *)buffer_tmp);
	}
	ESP_LOGD("ATouch", "%s",buffer_log);
}

void printf_byte_str(uint8_t *buf,uint16_t len)
{
	uint8_t buffer_tmp[32];
	uint8_t buffer_log[1024];	
	uint16_t count = 0;
	buffer_log[0] = '\0';

	for(;count < len;count++)
	{
		sprintf((char *)buffer_tmp, "%c",*(buf + count));
		strcat((char *)buffer_log,(const char *)buffer_tmp);
	}
	ESP_LOGD("ATouch", "%s",buffer_log);
}