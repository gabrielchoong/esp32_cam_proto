#include "dashcam.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"

void dashcam_info_t(dashcam_t *out_info)
{
	out_info->revision = 100;
}

void dashcam_init()
{
	static const char *TAG = "init:dashcam_esp32";

	// dashcam info dump
	dashcam_t dashcam_info;
	dashcam_info_t(&dashcam_info);
	unsigned major_dashcam_rev = dashcam_info.revision / 100;
	unsigned minor_dashcam_rev = dashcam_info.revision % 100;

	// esp32 chip info dump
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	unsigned major_rev = chip_info.revision / 100;
	unsigned minor_rev = chip_info.revision % 100;

	uint32_t flash_size;
	if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
		ESP_LOGE(TAG, "Get flash size failed");
		return;
	}

	ESP_LOGI(
		TAG,
		"This is PT-DC001 v%d.%d with %s chip with %d CPU core(s) silicon revision v%d.%d "
		"%" PRIu32 "MB %s flash\n",
		major_dashcam_rev, minor_dashcam_rev, CONFIG_IDF_TARGET,
		chip_info.cores, major_rev, minor_rev,
		flash_size / (uint32_t)(1024 * 1024),
		(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" :
								"external");
}
