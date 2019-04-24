#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_wifi.h"

#include "usb_hub.h"
#include "esp_bluetooth.h"
#include "adb_device.h"

void app_main()
{
    esp_bluetooth_init();

    //xTaskCreate(bt_send_task, "bt_send_task", 4*1024, NULL, 1, NULL);
    xTaskCreate(usb_hub_task, "usb_hub_task", 4*1024, NULL, 0, NULL);

    while(1) {
        //printf("cnt: %d\n", cnt++);
        vTaskDelay(90000 / portTICK_RATE_MS);
    }
}

