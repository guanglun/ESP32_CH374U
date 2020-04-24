#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "log.h"
#include "ch374u_app.h"
#include "ch374u_hal.h"
#include "adb_device.h"
#include "adb_protocol.h"
#include "CH374INC.H"
#include "scmd.h"

#include "esp_bluetooth.h"
#include "esp_wifi_station.h"
#include "msg_send.h"


uint8_t test_count = 0;
uint8_t send_count = 0;
uint8_t send_temp[256];
uint8_t send_temp_len = 0;
uint8_t send_lock = 0;
signed int x = 0, y = 0;

uint8_t msg_send(uint8_t *buf, uint16_t len, uint8_t dev_class)
{
    //static uint8_t is_send_flag = 0;
    unsigned char buf_tmp[100];
    unsigned char send_len = 0;

    if (adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS && is_tcp_send_done == true)
    {
        is_tcp_send_done = false;

        if (dev_class == 0x00)
        {

            send_len = cmd_creat(0x00, buf, len, buf_tmp);
            send_tcpserver(local_id, remote_id, buf_tmp, send_len);

            ESP_LOGD("ATouch", "ADB TCP Status: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_MOUSE)
        {
            send_len = cmd_creat(0x02, buf, len, buf_tmp);
            send_tcpserver(local_id, remote_id, buf_tmp, send_len);

            ESP_LOGD("ATouch", "ADB TCP Mouse: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_KEYBOARD)
        {
            send_len = cmd_creat(0x03, buf, len, buf_tmp);
            send_tcpserver(local_id, remote_id, buf_tmp, send_len);

            ESP_LOGD("ATouch", "ADB TCP KeyBoard: ");
            printf_byte(buf_tmp, send_len);
        }
        return 0;
    }else if(is_wifi_socket_connect == true)
    {
        if (dev_class == 0x00)
        {

            send_len = cmd_creat(0x00, buf, len, buf_tmp);
            wifi_socket_send((char *)buf_tmp, send_len);

            ESP_LOGD("ATouch", "WIFI TCP Status: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_MOUSE)
        {
            send_len = cmd_creat(0x02, buf, len, buf_tmp);
            wifi_socket_send((char *)buf_tmp, send_len);

            ESP_LOGD("ATouch", "WIFI TCP Mouse: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_KEYBOARD)
        {
            send_len = cmd_creat(0x03, buf, len, buf_tmp);
            wifi_socket_send((char *)buf_tmp, send_len);

            ESP_LOGD("ATouch", "WIFI TCP KeyBoard: ");
            printf_byte(buf_tmp, send_len);
        }
        return 0;
    }
    else if (get_bluetooth_status() == 1)
    {
        if (dev_class == 0x00)
        {
            send_len = cmd_creat(0x00, buf, len, buf_tmp);
            esp_bluetooth_send(buf_tmp, send_len);
#ifdef BLE_LOG
            ESP_LOGD("ATouch", "BLUE Status: ");
            printf_byte(buf_tmp, send_len);
#endif
        }
        else if (dev_class == DEV_MOUSE)
        {
            test_count++;
            buf[len - 1] = test_count;

            send_len = cmd_creat(0x02, buf, len, buf_tmp);
            esp_bluetooth_send(buf_tmp, send_len);

            ESP_LOGD("ATouch", "BLUE Mouse: ");
            //ESP_LOGI("ATouch", "%d %d ", (signed char)buf[1], (signed char)buf[2]);
            printf_byte(buf_tmp, send_len);

            //         send_count = 0;
            // }
        }
        else if (dev_class == DEV_KEYBOARD)
        {
            send_len = cmd_creat(0x03, buf, len, buf_tmp);
            esp_bluetooth_send(buf_tmp, send_len);

            ESP_LOGD("ATouch", "BLUE KeyBoard: ");
            printf_byte(buf_tmp, send_len);
        }
        return 0;
    }
    else
    {
        return 1;
    }
}