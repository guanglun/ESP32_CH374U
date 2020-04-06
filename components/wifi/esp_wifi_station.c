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
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "esp_wifi_station.h"
#include "log.h"

#include "usb_hub.h"

#include "nvs_flash.h"
#include "nvs.h"

bool is_wifi_socket_connect = false;

struct WIFI_INFO wifi_info;

#define EXAMPLE_ESP_MAXIMUM_RETRY 10
#define CLIENT_PORT 1989

#define CONFIG_EXAMPLE_IPV4

static EventGroupHandle_t s_wifi_event_group;
static const char *TAG = "wifi station";
const int WIFI_CONNECTED_BIT = BIT0;
static int s_retry_num = 0;
int sock = 0;
static void tcp_client_task(void *pvParameters);

void nvs_read_wifi(void);
void nvs_write_wifi(void);

void set_wifi_info(char *str)
{
    str = strstr(str,"[WIFI]");

    if(str == NULL)
        return ;
    
    str += 5;
    int i = 0, ii = 0;
    char flg = 0;
    while (str[i++] != '\0')
    {
        if (flg == 0 && str[i] != ';')
        {
            wifi_info.ssid[ii++] = str[i];
        }
        else if (flg == 0 && str[i] == ';')
        {
            flg = 1;
            wifi_info.ssid[ii] = '\0';
            ii = 0;
        }
        else if (flg == 1 && str[i] != ';')
        {
            wifi_info.passwd[ii++] = str[i];
        }
        else if (flg == 1 && str[i] == ';')
        {
            flg = 2;
            wifi_info.passwd[ii] = '\0';
            ii = 0;
        }
        else if (flg == 2 && str[i] != ';')
        {
            wifi_info.ip[ii++] = str[i];
        }
    }

    wifi_info.ip[ii] = '\0';

    ESP_LOGI(TAG, "SSID: %s", wifi_info.ssid);
    ESP_LOGI(TAG, "PASD: %s", wifi_info.passwd);
    ESP_LOGI(TAG, "IP:   %s", wifi_info.ip);

    ESP_ERROR_CHECK(esp_wifi_stop());

    nvs_write_wifi();

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    memcpy(wifi_config.sta.ssid, wifi_info.ssid, strlen((const char *)wifi_info.ssid));
    memcpy(wifi_config.sta.password, wifi_info.passwd, strlen((const char *)wifi_info.passwd));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_socket_send(char *buf,uint16_t len)
{
    if(is_wifi_socket_connect == true)
    {
        int err = send(sock, buf, len, 0);
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
        }
    }

}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        ESP_LOGI(TAG, "connect to the AP fail\n");
        break;
    }
    default:
        break;
    }
    return ESP_OK;
}

static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;

    while (1)
    {

#ifdef CONFIG_EXAMPLE_IPV4
        struct sockaddr_in destAddr;
        destAddr.sin_addr.s_addr = inet_addr((char *)wifi_info.ip);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(CLIENT_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else // IPV6
        struct sockaddr_in6 destAddr;
        inet6_aton(wifi_info.ip, &destAddr.sin6_addr);
        destAddr.sin6_family = AF_INET6;
        destAddr.sin6_port = htons(CLIENT_PORT);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
        inet6_ntoa_r(destAddr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

        sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        }
        ESP_LOGI(TAG, "Successfully connected");
        is_wifi_socket_connect = true;
        set_status(3, 1);
        while (1)
        {

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occured during receiving
            if (len < 0)
            {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else if(len > 0)
            {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }

            //vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        is_wifi_socket_connect = false;
        set_status(3, 0);
        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
void wifi_init_station(void)
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    nvs_read_wifi();

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    memcpy(wifi_config.sta.ssid, wifi_info.ssid, strlen((const char *)wifi_info.ssid));
    memcpy(wifi_config.sta.password, wifi_info.passwd, strlen((const char *)wifi_info.passwd));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void nvs_write_wifi(void)
{
    nvs_handle handle;
    static const char *NVS_CUSTOMER = "customer data";

    ESP_ERROR_CHECK( nvs_open( NVS_CUSTOMER, NVS_READWRITE, &handle) );
    ESP_ERROR_CHECK( nvs_set_str( handle, "wifi_ssid", (char *)wifi_info.ssid) );
    ESP_ERROR_CHECK( nvs_set_str( handle, "wifi_passwd", (char *)wifi_info.passwd) );
    ESP_ERROR_CHECK( nvs_set_str( handle, "wifi_ip", (char *)wifi_info.ip) );

    ESP_ERROR_CHECK( nvs_commit(handle) );
    nvs_close(handle);

}

void nvs_read_wifi(void)
{
    nvs_handle handle;
    static const char *NVS_CUSTOMER = "customer data";
    

    ESP_ERROR_CHECK( nvs_open(NVS_CUSTOMER, NVS_READWRITE, &handle) );

    uint32_t str_length = 33;
    ESP_ERROR_CHECK ( nvs_get_str(handle, "wifi_ssid", (char *)wifi_info.ssid, &str_length) );
    str_length = 64;
    ESP_ERROR_CHECK ( nvs_get_str(handle, "wifi_passwd", (char *)wifi_info.passwd, &str_length) );
    str_length = 16;
    ESP_ERROR_CHECK ( nvs_get_str(handle, "wifi_ip", (char *)wifi_info.ip, &str_length) );

    ESP_LOGI(TAG, "SSID: %s", wifi_info.ssid);
    ESP_LOGI(TAG, "PASD: %s", wifi_info.passwd);
    ESP_LOGI(TAG, "IP:   %s", wifi_info.ip);

    nvs_close(handle);
}