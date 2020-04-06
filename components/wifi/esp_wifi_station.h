#ifndef __ESP_WIFI_STATION_H__
#define __ESP_WIFI_STATION_H__

extern bool is_wifi_socket_connect;

struct WIFI_INFO
{
    unsigned char ssid[33];
    unsigned char passwd[65];
    unsigned char ip[16];
};

void wifi_init_station(void);
void set_wifi_info(char *str);
void wifi_socket_send(char *buf,uint16_t len);

#endif
