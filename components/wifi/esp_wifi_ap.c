#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "esp_wifi_ap.h"
#include "upgrade.h"
#include "log.h"

#define SERVER_PORT 4756
#define EXAMPLE_ESP_WIFI_SSID "ATouchUpgrade"
#define EXAMPLE_ESP_WIFI_PASS "123456789"
#define EXAMPLE_MAX_STA_CONN 1

void tcp_server_task(void *pvParameters);

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_AP_STACONNECTED:
        printf("station:" MACSTR " join, AID=%d\r\n",
               MAC2STR(event->event_info.sta_connected.mac),
               event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        printf("station:" MACSTR "leave, AID=%d\r\n",
               MAC2STR(event->event_info.sta_disconnected.mac),
               event->event_info.sta_disconnected.aid);
        break;
    default:
        break;
    }
    return ESP_OK;
}

void wifi_init_softap(void)
{
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("wifi_init_softap finished.SSID:%s password:%s\r\n",
           EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

    xTaskCreate(tcp_server_task, "tcp_server", 8192, NULL, 5, NULL);
}

void tcp_server_task(void *pvParameters)
{
    uint8_t rx_buffer[2048];
    char addr_str[128];
    int addr_family;
    int ip_protocol;
    int size = 0;
    int recv_len = 0;
    int recv_count = 0;
    while (1)
    {

        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(SERVER_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0)
        {
            printf("Unable to create socket: errno %d\r\n", errno);
            break;
        }
        printf("Socket created\r\n");

        int err = bind(listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0)
        {
            printf("Socket unable to bind: errno %d\r\n", errno);
            break;
        }
        printf("Socket binded");

        err = listen(listen_sock, 1);
        if (err != 0)
        {
            printf("Error occured during listen: errno %d\r\n", errno);
            break;
        }
        printf("Socket listening\r\n");

        while (1)
        {
            struct sockaddr_in6 sourceAddr; // Large enough for both IPv4 or IPv6
            uint addrLen = sizeof(sourceAddr);
            int sock = accept(listen_sock, (struct sockaddr *)&sourceAddr, &addrLen);
            if (sock < 0)
            {
                printf("Unable to accept connection: errno %d\r\n", errno);
                break;
            }
            printf("Socket accepted\r\n");

            recv_len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occured during receiving
            if (recv_len == 8)
            {
                printf("recv\r\n");
                printf_byte(rx_buffer, recv_len);

                if (((rx_buffer[0] == 0x00) && (rx_buffer[1] == 0x01) && (rx_buffer[2] == 0x02) && (rx_buffer[3] == 0x03)) != 0)
                {
                    size = 0;
                    size |= (rx_buffer[4] << 24);
                    size |= (rx_buffer[5] << 16);
                    size |= (rx_buffer[6] << 8);
                    size |= (rx_buffer[7] << 0);
                    printf("\r\nfile size:%d\r\n", size);
                    recv_count = 0;
                    upgrade_start();
                    int err = send(sock, rx_buffer, 8, 0);
                    if (err < 0)
                    {
                    }
                }
            }
            else if (recv_len == 0)
            {
                printf("Connection closed\r\n");
                break;
            }

            while (recv_count < size)
            {
                recv_len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                recv_count += recv_len;
                upgrade_write((char *)rx_buffer, recv_len);
                printf("recv %d %d %d ", size, recv_count, recv_len);
                printf_byte(rx_buffer, 10);
                printf("\r\n");

                int err = send(sock, rx_buffer, 8, 0);
                if (err < 0)
                {
                }
            }

            if(upgrade_end() == ESP_OK)
            {
                rx_buffer[0] = 'o';
                rx_buffer[1] = 'k';
                send(sock, rx_buffer, 2, 0);

                printf("Prepare to restart system!\r\n");

                vTaskDelay(1000 / portTICK_RATE_MS);

                printf("restart system!\r\n");
                esp_restart();    
                
            }else{
                rx_buffer[0] = 'f';
                rx_buffer[1] = 'a';
                rx_buffer[2] = 'i';
                rx_buffer[3] = 'l';
                send(sock, rx_buffer, 4, 0);
            }


            if (sock != -1)
            {
                printf("Shutting down socket and restarting...\r\n");
                shutdown(sock, 0);
                close(sock);
            }
        }
    }
    vTaskDelete(NULL);
}