#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "sht31.h"
#include "connect_wifi.h"
#include "config.h"

#define LED_PIN 2
#define INDEX_HTML_PATH "/spiffs/index.html"

const char index_html[4096];
const char response_data[4096];

int led_state = 0;

TaskHandle_t TASK_READ_SHT31 = NULL;
TaskHandle_t TASK_READ_TEMP = NULL;
TaskHandle_t TASK_READ_HUMI = NULL;

static const char *TAG = "Main";
float t, h;

static void initi_web_page_buffer(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    memset((void *)index_html, 0, sizeof(index_html));
    struct stat st;
    if (stat(INDEX_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "index.html not found");
        return;
    }

    FILE *fp = fopen(INDEX_HTML_PATH, "r");
    if (fread(index_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    fclose(fp);
}

esp_err_t send_web_page(httpd_req_t *req)
{
    int response;
    char *p = (char *)malloc(10 * sizeof(char));  // cấp phát đủ bộ nhớ cho con trỏ p
    char *pt = (char *)malloc(10 * sizeof(char)); // cấp phát đủ bộ nhớ cho con trỏ pt
    sprintf(p, "%2.2f", t);
    sprintf(pt, "%2.2f", h);
    sprintf(response_data, index_html, p, pt);
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    free(p);
    free(pt);
    return response;
}
esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

esp_err_t led_on_handler(httpd_req_t *req)
{
    // gpio_set_level(LED_PIN, 1);
    // led_state = 1;
    printf("Light On\n");
    return 0;
}

esp_err_t led_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN, 0);
    led_state = 0;
    return send_web_page(req);
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL};

httpd_uri_t uri_on = {
    .uri = "/lighton",
    .method = HTTP_GET,
    .handler = led_on_handler,
    .user_ctx = NULL};

httpd_uri_t uri_off = {
    .uri = "/led2off",
    .method = HTTP_GET,
    .handler = led_off_handler,
    .user_ctx = NULL};

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_on);
        // httpd_register_uri_handler(server, &uri_off);
    }

    return server;
}

void Get_SHT31(void *params)
{
    while (1)
    {
        if (sht31_readTempHum())
        {
            t = sht31_GetTemperature();
            h = sht31_GetHumidity();
        }
        else
        {
            ESP_LOGE(TAG, "Read SHT31 failed");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void Get_Temp(void *params)
{
    while (1)
    {
        printf("Temp: %2.2f \n", t);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
void Get_Humi(void *params)
{
    while (1)
    {
        printf("Humi: %2.2f \n", h);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    CFG_Spiffs_Init();
    connect_wifi();

}
