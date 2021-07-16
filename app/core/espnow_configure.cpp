#include <iostream>
#include <cstring>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include "nvs_flash.h"

#include "espnow_configure.hpp"

// 10:52:1c:5d:f8:ac
namespace ens
{
    uint8_t *msg = new uint8_t[3];
    const static uint8_t my_mac[6] = {0x10,0x52,0x1c,0x5d,0xf8,0xac};
    const static uint8_t other_mac[6] = {0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
    static void wifi_init()
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());

    }
    void send()
    {
        // const uint8_t _item = 0xff;
        // const uint8_t mac_addr[6] = { _item, _item, _item, _item, _item, _item };
        esp_now_peer_info_t peer_info = {};
        memcpy(&peer_info.peer_addr, other_mac, ESP_NOW_ETH_ALEN);
        if(!esp_now_is_peer_exist(other_mac))
        {
            esp_now_add_peer(&peer_info);
        }
        const uint8_t arr[3] = {0xae, 0x09, 0x77};
        

        const auto& status = esp_now_send(peer_info.peer_addr, 
                                    arr,
                                    3);
        if(status != ESP_OK)
        {
            std::cout<<"Error occured. Status: "<<static_cast<int>(status)<<std::endl;
            return;
        }
        std::cout<<"Succeded broadcasting: \nMAC ";
        for(int i = 0; i<6; i++)
        {
            std::cout<<std::hex<<static_cast<int>(peer_info.peer_addr[i])<<":";
        }
        std::cout<<"\nMessage: ";
        for(int i = 0; i<3; i++)
        {
            std::cout<<static_cast<int>(arr[i])<<" ";
        }
        std::cout<<"\nStatus: "<<status<<std::endl;
    }
    void output_msg(const uint8_t* mac, const uint8_t* buf, int len)
    {
        std::cout<<"From: ";
        for(int i = 0; i < 6; i++)
        {
            std::cout<<std::hex<<static_cast<int>(mac[i])<<":";
        }
        std::cout<<" recieved message: ";
        for(int i = 0; i < len; i++)
        {
            std::cout<<std::dec<<static_cast<int>(buf[i]);
        }
        std::cout<<std::endl;
    }

    void onDataRecieve(const unsigned char* mac, const uint8_t* income_message, int len)
    {
        memcpy(msg, income_message, len);
        output_msg(mac, msg, len);
    }

}// namespace ens(esp_now setup)

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ens::wifi_init();
    esp_base_mac_addr_set(ens::my_mac);
    ESP_ERROR_CHECK( esp_now_init() );
    while(true)
    {
        ens::send();
        vTaskDelay(500);
    }
    // while(esp_now_register_recv_cb(ens::onDataRecieve) != ESP_OK)
    // {
    //     std::cout<<"Awaiting.."<<std::endl;
    //     vTaskDelay(500);
    // }
}
