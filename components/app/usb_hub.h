#ifndef __USB_HUB_H__
#define __USB_HUB_H__

void usb_hub_task(void* arg);
void set_status(uint8_t index,uint8_t value);

#endif
