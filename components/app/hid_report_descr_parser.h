#ifndef __HID_REPORT_DESCR_PARSER_H__
#define __HID_REPORT_DESCR_PARSER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

struct HID_MOUSE_REPORT_INDEX
{
    uint8_t button;
    uint8_t x;
    uint8_t y;
    uint8_t wheel;
    uint8_t value[4];
};

int hid_report_descr_parser(uint8_t *buff,uint16_t len);

#endif
