
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "log.h"

void printf_byte(uint8_t *buf,uint16_t len)
{
	uint16_t count = 0;
	for(;count < len;count++)
	{
		printf("%02X ",*(buf + count));
	}
	printf("\r\n");
}

void printf_byte_str(uint8_t *buf,uint16_t len)
{
	uint16_t count = 0;
	for(;count < len;count++)
	{
		printf("%c",*(buf + count));
	}
	printf("\r\n");
}