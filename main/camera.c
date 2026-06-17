#include "camera.h"
#include "esp_camera.h"
#include "esp_camera_af.h"
#include "esp_log.h"

#define BOARD_ESP32CAM_AITHINKER

#include "camera_pinout.h"

#define CONFIG_CAMERA_AF_DEFAULT_TIMEOUT_MS 100

static camera_config_t camera_config = {
	.pin_pwdn     = CAM_PIN_PWDN,
	.pin_reset    = CAM_PIN_RESET,
	.pin_xclk     = CAM_PIN_XCLK,
	.pin_sccb_sda = CAM_PIN_SIOD,
	.pin_sccb_scl = CAM_PIN_SIOC,
	.pin_d7	      = CAM_PIN_D7,
	.pin_d6	      = CAM_PIN_D6,
	.pin_d5	      = CAM_PIN_D5,
	.pin_d4	      = CAM_PIN_D4,
	.pin_d3	      = CAM_PIN_D3,
	.pin_d2	      = CAM_PIN_D2,
	.pin_d1	      = CAM_PIN_D1,
	.pin_d0	      = CAM_PIN_D0,
	.pin_vsync    = CAM_PIN_VSYNC,
	.pin_href     = CAM_PIN_HREF,
	.pin_pclk     = CAM_PIN_PCLK,

	.xclk_freq_hz = 20000000,
	.ledc_timer   = LEDC_TIMER_0,
	.ledc_channel = LEDC_CHANNEL_0,
	.pixel_format = PIXFORMAT_JPEG,
	.frame_size   = FRAMESIZE_SVGA,
	.jpeg_quality = 10,
	.fb_count     = 2,
	.grab_mode    = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init(void)
{
	esp_err_t err = esp_camera_init(&camera_config);
	if (err != ESP_OK) {
		return err;
	}
	return ESP_OK;
}

static void maybe_init_autofocus(void)
{
	static const char *TAG = "init:dashcam_esp32";
	sensor_t *s	       = esp_camera_sensor_get();
	if (!s) {
		ESP_LOGW(TAG, "AF: no sensor handle");
		return;
	}

	if (!esp_camera_af_is_supported(s)) {
		ESP_LOGI(TAG, "AF: not supported by this sensor");
		return;
	}

	esp_camera_af_config_t af_cfg = {
		.mode	    = ESP_CAMERA_AF_MODE_AUTO,
		.timeout_ms = CONFIG_CAMERA_AF_DEFAULT_TIMEOUT_MS,
	};

	esp_err_t ret = esp_camera_af_init(s, &af_cfg);
	if (ret != ESP_OK) {
		ESP_LOGW(TAG, "AF init failed: %s", esp_err_to_name(ret));
		return;
	}

	ESP_LOGI(TAG, "AF initialized (AUTO mode)");
}

void camera_init()
{
#if ESP_CAMERA_SUPPORTED
	init();
#if defined(CONFIG_CAMERA_AF_SUPPORT) && CONFIG_CAMERA_AF_SUPPORT
	maybe_init_autofocus();
#endif
#endif
}
