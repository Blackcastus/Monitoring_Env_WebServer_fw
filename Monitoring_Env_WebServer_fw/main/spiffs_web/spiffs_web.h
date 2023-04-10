/*
 *  Created on: 2023.05.04.
 *  Modifier on: 2023.10.04.
 *      Author: DucHien
 */

#ifndef SPIFFS_WEB_H_
#define SPIFFS_WEB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include <esp_log.h>
#include <esp_err.h>



#ifdef __cplusplus
}
#endif

#endif /* __SPIFFS_WEB_H_ */