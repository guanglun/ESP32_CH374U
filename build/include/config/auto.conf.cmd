deps_config := \
	/home/share/esp32/esp-idf-v3.2/components/app_trace/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/aws_iot/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/bt/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/driver/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/esp32/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/esp_adc_cal/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/esp_event/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/esp_http_client/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/esp_http_server/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/ethernet/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/fatfs/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/freemodbus/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/freertos/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/heap/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/libsodium/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/log/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/lwip/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/mbedtls/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/mdns/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/mqtt/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/nvs_flash/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/openssl/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/pthread/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/spi_flash/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/spiffs/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/tcpip_adapter/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/vfs/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/wear_levelling/Kconfig \
	/home/share/esp32/esp-idf-v3.2/components/bootloader/Kconfig.projbuild \
	/home/share/esp32/esp-idf-v3.2/components/esptool_py/Kconfig.projbuild \
	/home/share/esp32/esp-idf-v3.2/components/partition_table/Kconfig.projbuild \
	/home/share/esp32/esp-idf-v3.2/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
