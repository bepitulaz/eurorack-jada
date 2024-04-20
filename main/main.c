#include "esp_wifi.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "step_sequencer.h"

/* @brief tag used for ESP serial console messages */
static const char TAG[] = "main";

void cb_connection_ok(void *pvParameter){
	ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

	/* transform IP to human readable string */
	char str_ip[16];
	esp_ip4addr_ntoa(&param->ip_info.ip, str_ip, IP4ADDR_STRLEN_MAX);

	ESP_LOGI(TAG, "I have a connection and my IP is %s!", str_ip);
}

void app_main(void)
{
  wifi_manager_start();

  wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &cb_connection_ok);

	step_sequencer_init();
}
