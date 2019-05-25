#ifndef __UPGRADE_H__
#define __UPGRADE_H__

void upgrade_init(void);
esp_err_t upgrade_start(void);
esp_err_t upgrade_write(char *ota_write_data,uint32_t write_len);
esp_err_t upgrade_end(void);

#endif
