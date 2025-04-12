/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "sh2.h"
#include "bno08x.h"
#include "spp_server.h"

#include "time.h"
#include "sys/time.h"

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

// #define SPP_TAG 
#define SPP_SERVER_NAME "SPP_SERVER"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA    /*Choose show mode: show data or speed*/

static const char *TAG = "main";

static struct timeval time_new, time_old;
static long data_num = 0;

static uint32_t recent_handle = 0;

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    char bda_str[18] = {0};
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    TaskHandle_t imu_task;
    BaseType_t rtos_ret = xTaskCreate(
        bno_task, 
        "IMU Recorder", 
        40000 / sizeof(configSTACK_DEPTH_TYPE), 
        NULL, 
        configMAX_PRIORITIES - 2, 
        &imu_task
    );

    TaskHandle_t bt_server_task;
    rtos_ret = xTaskCreate(
        server_task,
        "Bluetooth Server",
        30000 / sizeof(configSTACK_DEPTH_TYPE), 
        NULL, 
        configMAX_PRIORITIES / 2, 
        &bt_server_task
    );

    // quat_t result;
    // QueueHandle_t imu_result_queue = bno_get_result_queue();
    // ESP_LOGD(TAG, "Starting main loop");
    // while (1) {
    //     if ((rtos_ret = xQueueReceive(imu_result_queue, &result, pdMS_TO_TICKS(1000))) != pdPASS) {
    //         ESP_LOGW(TAG, "No IMU results");
    //     } else {
    //         ESP_LOGD(TAG, "IMU result recieved: z = %f", result.z);
    //     }
    // }

    // if (create_ret == pdFAIL) {

    // } else if (create_ret == pdPASS) {

    // }
}
