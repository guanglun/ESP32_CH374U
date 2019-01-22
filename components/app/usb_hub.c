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

void usb_hub_task(void* arg)
{
    uint8_t inter_flag_reg = 0;
    
    Init374Host(); // 初始化USB主机

	printf("Wait Device In\n");

    while(1)
    {
        if (Query374Interrupt(&inter_flag_reg) == true)
		{
			HostDetectInterrupt(inter_flag_reg);
		}

		NewDeviceEnum();
        DeviceLoop();

        vTaskDelay(8/ portTICK_RATE_MS);
    }

    vTaskDelete(NULL);

}