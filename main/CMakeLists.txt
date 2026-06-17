idf_component_register(SRCS "main.c" "camera.c" "wifi.c" "dashcam.c"
		    PRIV_REQUIRES esp_wifi esp_http_server nvs_flash esp_netif
                    INCLUDE_DIRS "."
	    	    REQUIRES spi_flash)
