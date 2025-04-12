idf_component_register(
                    SRCS "bno08x.c" "main.c" "spp_server.c" "../sh2/euler.c" "../sh2/sh2_SensorValue.c" "../sh2/sh2_util.c" "../sh2/sh2.c" "../sh2/shtp.c"
                    PRIV_REQUIRES bt nvs_flash driver spiffs
                    INCLUDE_DIRS "." "../sh2")
