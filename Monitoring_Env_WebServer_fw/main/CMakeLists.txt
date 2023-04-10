idf_component_register(SRCS "main.c"
                            "SHT3x/sht31.c"
                            "connect_wifi/connect_wifi.c"
                            "config/config.c"
                    INCLUDE_DIRS "." "SHT3x" "connect_wifi" "config")
spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
