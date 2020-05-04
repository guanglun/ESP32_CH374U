#include "hid_report_descr_parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "log.h"

/*
 * HID report descriptor item type (prefix bit 2,3)
 */

#define HID_ITEM_TYPE_MAIN 0
#define HID_ITEM_TYPE_GLOBAL 1
#define HID_ITEM_TYPE_LOCAL 2
#define HID_ITEM_TYPE_RESERVED 3

/*
 * HID report descriptor main item tags
 */

#define HID_MAIN_ITEM_TAG_INPUT 8
#define HID_MAIN_ITEM_TAG_OUTPUT 9
#define HID_MAIN_ITEM_TAG_FEATURE 11
#define HID_MAIN_ITEM_TAG_BEGIN_COLLECTION 10
#define HID_MAIN_ITEM_TAG_END_COLLECTION 12

/*
 * HID report descriptor global item tags
 */

#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE 0
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM 1
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM 2
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM 3
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM 4
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT 5
#define HID_GLOBAL_ITEM_TAG_UNIT 6
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE 7
#define HID_GLOBAL_ITEM_TAG_REPORT_ID 8
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT 9
#define HID_GLOBAL_ITEM_TAG_PUSH 10
#define HID_GLOBAL_ITEM_TAG_POP 11

/*
 * HID report descriptor local item tags
 */

#define HID_LOCAL_ITEM_TAG_USAGE 0
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM 1
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM 2
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX 3
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM 4
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM 5
#define HID_LOCAL_ITEM_TAG_STRING_INDEX 7
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM 8
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM 9
#define HID_LOCAL_ITEM_TAG_DELIMITER 10

struct HID_REPORT
{
    uint8_t bTag;
    uint8_t bType;
    uint8_t bSize;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    } data;
};

static inline uint16_t __get_unaligned_le16(const uint8_t *p)
{
    return p[0] | p[1] << 8;
}
static inline uint32_t __get_unaligned_le32(const uint8_t *p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}
static inline uint16_t get_unaligned_le16(const void *p)
{
    return __get_unaligned_le16((const uint8_t *)p);
}

static inline uint32_t get_unaligned_le32(const void *p)
{
    return __get_unaligned_le32((const uint8_t *)p);
}

void hid_report_debug(uint8_t cmd,struct HID_REPORT *hid_r)
{
    switch (hid_r->bSize)
    {
    case 0:
        ESP_LOGI("ATouch", "%d\t%d\t%d\t%02X", hid_r->bTag, hid_r->bType, hid_r->bSize,cmd);
        break;
    case 1:
        ESP_LOGI("ATouch", "%d\t%d\t%d\t%02X\t%08X", hid_r->bTag, hid_r->bType, hid_r->bSize,cmd, hid_r->data.u32);
        break;
    case 2:
        ESP_LOGI("ATouch", "%d\t%d\t%d\t%02X\t%08X", hid_r->bTag, hid_r->bType, hid_r->bSize,cmd, hid_r->data.u32);
        break;
    case 3:
        ESP_LOGI("ATouch", "%d\t%d\t%d\t%02X\t%08X", hid_r->bTag, hid_r->bType, hid_r->bSize,cmd, hid_r->data.u32);
        break;
    }
}



struct HID_MOUSE_REPORT_INDEX hid_mouse_rep_index;

#define MAX_REPORT 40

int hid_report_descr_parser(uint8_t *buff, uint16_t len)
{
    bool is_enable_report_id = false;
    uint8_t is_start = 0,report_id = 0,index = 0;
    uint8_t type_flag = 0, size_flag = 0, count_flag = 0;
    uint8_t report_type[MAX_REPORT], report_size[MAX_REPORT], report_type_count = 0, report_size_count = 0;
    uint8_t cmd = 0, r_count = 0, r_size = 0;
    struct HID_REPORT hid_report[1024];
    uint16_t hid_report_count = 0;
    int i = 0, ii = 0;

    while (i < len)
    {
        hid_report[hid_report_count].bTag = (uint8_t)((buff[i] >> 4) & 0x0F);
        hid_report[hid_report_count].bType = (uint8_t)((buff[i] >> 2) & 0x03);
        hid_report[hid_report_count].bSize = (uint8_t)((buff[i] >> 0) & 0x03);

        cmd = buff[i];

        i++;

        switch (hid_report[hid_report_count].bSize)
        {
        case 0:
            break;
        case 1:
            hid_report[hid_report_count].data.u32 = buff[i];
            i += 1;
            break;
        case 2:
            hid_report[hid_report_count].data.u32 = get_unaligned_le16(buff);
            i += 2;
            break;
        case 3:
            hid_report[hid_report_count].data.u32 = get_unaligned_le32(buff);
            i += 4;
            break;
        }

        if ((cmd == 0x09) && (hid_report[hid_report_count].data.u32 == 0x02))
        {
            is_start++;
        }else if ((cmd == 0xA1) && (hid_report[hid_report_count].data.u32 == 0x01))
        {
            is_start++;
        }else if ((cmd == 0x09) && (hid_report[hid_report_count].data.u32 == 0x01))
        {
            is_start++;
        }else if ((cmd == 0xA1) && (hid_report[hid_report_count].data.u32 == 0x00))
        {
            is_start++;
        }else if ((cmd == 0xC0))
        {
            is_start = 0;
        }

        if (is_start >= 4)
        {
            switch (cmd)
            {
            case 0x85:
                is_enable_report_id = true;
                report_id = hid_report[hid_report_count].data.u32;
            break;
            case 0x75:
                r_size = hid_report[hid_report_count].data.u32;
                size_flag++;
                break;
            case 0x95:
                r_count = hid_report[hid_report_count].data.u32;
                count_flag++;
                break;
            case 0x09:
                report_type[report_type_count++] = hid_report[hid_report_count].data.u32;
                type_flag++;
                break;
            case 0x05:
                if(hid_report[hid_report_count].data.u32 == 0x09)
                {
                    report_type[report_type_count++] = hid_report[hid_report_count].data.u32;
                    type_flag++;
                }

                break;                
            case 0x81:
                if (type_flag == 0)
                {
                    for (ii = 0; ii < r_count; ii++)
                        report_type[report_type_count++] = 0xFF;
                }

                if (report_type[report_type_count - 1] == 0x09) //指针按键
                {
                    report_size[report_size_count++] = r_count;
                }
                else if (count_flag > 0)
                {
                    for (ii = 0; ii < r_count; ii++)
                    {
                        report_size[report_size_count++] = r_size;
                    }
                }

                count_flag = 0;
                type_flag = 0;
                size_flag = 0;
                break;
            default:
                break;
            }
        }

        //hid_report_debug(cmd,&hid_report[hid_report_count]);

        hid_report_count++;
    }

    ESP_LOGI("ATouch", "\t\t\t\t========HID Report=======");

    if(is_enable_report_id == true)
    {
        ESP_LOGI("ATouch", "\t\t\t\tReport ID: %02X", report_id);
        index += 8;
    }
    for (ii = 0; ii < report_type_count; ii++)
    {
        ESP_LOGI("ATouch", "\t\t\t\t%02X\t%d", report_type[ii], report_size[ii]);

        if(report_type[ii] == 0x09)
        {
            hid_mouse_rep_index.button = index/8;
        }else if(report_type[ii] == 0x30)
        {
            hid_mouse_rep_index.x = index/8;
        }else if(report_type[ii] == 0x31)
        {
            hid_mouse_rep_index.y = index/8;
        }else if(report_type[ii] == 0x38)
        {
            hid_mouse_rep_index.wheel = index/8;
        }

        index += report_size[ii];
    }
    ESP_LOGI("ATouch", "\t\t\t\t========HID Report=======");

    if(report_type_count > 0)
        return 0;
    
    return -1;
}