#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "log.h"
#include "led_hal.h"

void led_init(void)
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = LED_CTRL_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1U << KEY_INPUT);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    LED_STATUS_HIGH();
    LED_USB0_HIGH();
    LED_USB1_HIGH();
    LED_USB2_HIGH();
}

void led_status_turn(void)
{
    if(LED_STATUS_READ() == 0x01)
    {
        LED_STATUS_LOW();
    }else {
        LED_STATUS_HIGH();
    }
}

void led_usb0_turn(void)
{
    if(LED_USB0_READ() == 0x01)
    {
        LED_USB0_LOW();
    }else {
        LED_USB0_HIGH();
    }
}

void led_usb1_turn(void)
{
    if(LED_USB1_READ() == 0x01)
    {
        LED_USB1_LOW();
    }else {
        LED_USB1_HIGH();
    }
}

void led_usb2_turn(void)
{
    if(LED_USB2_READ() == 0x01)
    {
        LED_USB2_LOW();
    }else {
        LED_USB2_HIGH();
    }
}

