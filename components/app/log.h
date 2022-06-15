#ifndef __LOG_H__
#define __LOG_H__

#include "esp_log.h"

void printf_byte(uint8_t *buf,uint16_t len);
void printf_byte_str(uint8_t *buf,uint16_t len);
void printf_byte_no_esp_log(uint8_t *buf,uint16_t len);
void printf_byte_logi(uint8_t *buf,uint16_t len);

#endif
