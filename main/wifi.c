#include "wifi.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define WIFI_SSID "PT-DC001"
#define WIFI_PASS "dashcam1"

static const char *TAG = "";

httpd_handle_t start_webserver()
{
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	httpd_handle_t server = NULL;
	if (httpd_start(&server, &config) != ESP_OK) {
		ESP_LOGE(TAG, "Failed to start server");
	}

	ESP_LOGI(TAG,
		 "Server started successfully, registering URI handlers...");
	return server;
};

static esp_err_t capture_handler(httpd_req_t *req)
{
	camera_fb_t *pic = esp_camera_fb_get();
	if (!pic) {
		httpd_resp_send_500(req);
		return ESP_FAIL;
	}
	httpd_resp_set_type(req, "image/jpeg");
	httpd_resp_send(req, (const char *)pic->buf, pic->len);
	esp_camera_fb_return(pic);
	return ESP_OK;
}

static const httpd_uri_t config = {
	.uri	  = "/capture",
	.method	  = HTTP_GET,
	.handler  = capture_handler,
	.user_ctx = NULL,
};

void wifi_init_softap(void)
{
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .ssid_len = strlen(WIFI_SSID),
            .max_connection = 1,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

	esp_wifi_set_mode(WIFI_MODE_AP);
	esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
	esp_wifi_start();

	ESP_LOGI(TAG, "SoftAP started. SSID: %s", WIFI_SSID);
}

void wifi_start(void)
{
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
		nvs_flash_erase();
		nvs_flash_init();
	}

	esp_netif_init();
	esp_event_loop_create_default();
	esp_netif_create_default_wifi_ap();
	wifi_init_softap();

	httpd_handle_t server = start_webserver();
	httpd_register_uri_handler(server, &config);
}
