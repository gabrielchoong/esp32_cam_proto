#include "camera.h"
#include "dashcam.h"
#include "wifi.h"

void app_main(void)
{
	dashcam_init();
	camera_init();
	wifi_start();
}
