#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"

#include "esp_wifi_ap.h"
#include "upgrade.h"

#define HASH_LEN 32

esp_ota_handle_t update_handle = 0;
const esp_partition_t *update_partition = NULL;

void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    printf("%s: %s\r\n", label, hash_print);
}

void upgrade_init(void)
{

    uint8_t sha_256[HASH_LEN] = {0};
    esp_partition_t partition;

    // get sha256 digest for the partition table
    partition.address = ESP_PARTITION_TABLE_OFFSET;
    partition.size = ESP_PARTITION_TABLE_MAX_LEN;
    partition.type = ESP_PARTITION_TYPE_DATA;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for the partition table: \r\n");

    // get sha256 digest for bootloader
    partition.address = ESP_BOOTLOADER_OFFSET;
    partition.size = ESP_PARTITION_TABLE_OFFSET;
    partition.type = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: \r\n");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: \r\n");

    wifi_init_softap();
}

esp_err_t upgrade_start(void)
{
    esp_err_t err;
    /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
    
    printf("Starting OTA ...");

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running)
    {
        printf("Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x\r\n",
               configured->address, running->address);
        printf("(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)\r\n");
    }
    printf("Running partition type %d subtype %d (offset 0x%08x)\r\n",
           running->type, running->subtype, running->address);

    update_partition = esp_ota_get_next_update_partition(NULL);
    printf("Writing to partition subtype %d at offset 0x%x\r\n",
           update_partition->subtype, update_partition->address);
    assert(update_partition != NULL);

    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK)
    {
        printf("esp_ota_begin failed (%s)\r\n", esp_err_to_name(err));
        return ESP_FAIL;
    }
    printf("esp_ota_begin succeeded\r\n");
    return ESP_OK;
}

esp_err_t upgrade_write(char *ota_write_data,uint32_t write_len)
{
    esp_err_t err;
    err = esp_ota_write(update_handle, (const void *)ota_write_data, write_len);
    return err;
}

esp_err_t upgrade_end(void)
{
    esp_err_t err;
    if (esp_ota_end(update_handle) != ESP_OK)
    {
        printf("esp_ota_end failed!\r\n");
        return ESP_FAIL;
    }

    if (esp_partition_check_identity(esp_ota_get_running_partition(), update_partition) == true)
    {
        printf("The current running firmware is same as the firmware just downloaded\r\n");
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK)
    {
        printf("esp_ota_set_boot_partition failed (%s)!\r\n", esp_err_to_name(err));
        return ESP_FAIL;
    }

    // printf("Prepare to restart system!\r\n");
    // esp_restart();    

    return ESP_OK;
}