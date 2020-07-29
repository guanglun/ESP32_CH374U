#ifndef __LED_HAL_H__
#define __LED_HAL_H__

#define LED_STATUS 4
#define LED_USB0 25
#define LED_USB1 26
#define LED_USB2 27
#define KEY_INPUT 2

#define LED_CTRL_SEL ((1ULL << LED_STATUS) | (1ULL << LED_USB0) | (1ULL << LED_USB1) | (1ULL << LED_USB2))

#define LED_STATUS_HIGH()   GPIO.out_w1ts = (1 << LED_STATUS)
#define LED_STATUS_LOW()    GPIO.out_w1tc = (1 << LED_STATUS)
#define LED_STATUS_READ()   ((0x01) & (GPIO.in >> LED_STATUS))

#define LED_USB0_HIGH()     GPIO.out_w1ts = (1 << LED_USB0)
#define LED_USB0_LOW()      GPIO.out_w1tc = (1 << LED_USB0)
#define LED_USB0_READ()     ((0x01) & (GPIO.in >> LED_USB0))

#define LED_USB1_HIGH()     GPIO.out_w1ts = (1 << LED_USB1)
#define LED_USB1_LOW()      GPIO.out_w1tc = (1 << LED_USB1)
#define LED_USB1_READ()     ((0x01) & (GPIO.in >> LED_USB1))

#define LED_USB2_HIGH()     GPIO.out_w1ts = (1 << LED_USB2)
#define LED_USB2_LOW()      GPIO.out_w1tc = (1 << LED_USB2)
#define LED_USB2_READ()     ((0x01) & (GPIO.in >> LED_USB2))

#define KEY_READ()          ((0x01) & (GPIO.in >> KEY_INPUT))

void led_init(void);
void led_status_turn(void);
void led_usb0_turn(void);
void led_usb1_turn(void);
void led_usb2_turn(void);

#endif