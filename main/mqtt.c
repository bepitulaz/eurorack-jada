#include "mqtt.h"
#include "mqtt_client.h"
#include "credentials.h"

static const uint8_t mqtt_hivemq_pem_start[] = "-----BEGIN CERTIFICATE-----\nMIIFCTCCA/GgAwIBAgISA/z48Z/1VKLmoZyQsSlGeYqSMA0GCSqGSIb3DQEBCwUA MDIxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQD EwJSMzAeFw0yNDA0MjgwMjI5MThaFw0yNDA3MjcwMjI5MTdaMB8xHTAbBgNVBAMM FCouczEuZXUuaGl2ZW1xLmNsb3VkMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB CgKCAQEApW7Paww+bHHbD9/zX9g\n-----END CERTIFICATE-----";

void mqtt_app_start()
{
  esp_mqtt_client_config_t mqtt_cfg = {
    .broker = {
      .address.uri = MQTT_BROKER_URI,
      .verification.certificate = (const char *)mqtt_hivemq_pem_start,
    },
    .credentials = {
      .username = MQTT_USERNAME,
      .client_id = "JADA_%CHIPID%",
      .authentication.password = MQTT_PASSWORD,
    },
  };

  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
}
