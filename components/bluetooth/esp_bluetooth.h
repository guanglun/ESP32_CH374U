#ifndef __ESP_BLUETOOTH_H__
#define __ESP_BLUETOOTH_H__

void esp_bluetooth_init(void);
int esp_bluetooth_send(uint8_t *buf,int len);
char get_bluetooth_status(void);

#endif
