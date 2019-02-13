#ifndef __BLUFI_SECURITY_H__
#define __BLUFI_SECURITY_H__

#define BLUFI_EXAMPLE_TAG "BLUFI_EXAMPLE"
#define BLUFI_INFO(fmt, ...)   ESP_LOGI(BLUFI_EXAMPLE_TAG, fmt, ##__VA_ARGS__) 
#define BLUFI_ERROR(fmt, ...)  ESP_LOGE(BLUFI_EXAMPLE_TAG, fmt, ##__VA_ARGS__) 

int blufi_security_init(void);
void blufi_security_deinit(void);

#endif