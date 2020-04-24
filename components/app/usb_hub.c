#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"

#include "log.h"
#include "ch374u_app.h"
#include "ch374u_hal.h"
#include "adb_device.h"
#include "adb_protocol.h"
#include "CH374INC.H"
#include "msg_send.h"

uint8_t status_buf[4] = {0,0,0,0};

void set_status(uint8_t index,uint8_t value)
{
    if(index < 4)
    {
        status_buf[index] = value;
    }
    
}


void usb_hub_task(void* arg)
{
    uint8_t inter_flag_reg = 0;
    uint16_t timer_count = 0;
    Init374Host(); // 初始化USB主机

	printf("Wait Device In\n");

    while(1)
    {
        timer_count++;

        if (Query374Interrupt(&inter_flag_reg) == true)
		{
			HostDetectInterrupt(inter_flag_reg);
		}

		NewDeviceEnum();
        DeviceLoop();

        if(timer_count >= 100)
        {
            timer_count = 0;
            msg_send(status_buf,4,0x00);
        }

        vTaskDelay(10/ portTICK_RATE_MS);
    }

    vTaskDelete(NULL);

}