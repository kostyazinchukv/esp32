set(SOURCES
    espnow_configure.cpp
    )
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES esp_wifi esp_netif esp_event nvs_flash)