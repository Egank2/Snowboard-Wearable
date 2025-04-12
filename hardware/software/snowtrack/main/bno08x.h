#ifndef BNO08X_H
#define BNO08X_H

#include "esp_err.h"
#include "sh2_SensorValue.h"

typedef struct {
    float w;
    float x;
    float y;
    float z;
} quat_t;

esp_err_t bno_init();

bool bno_getSensorEvent(sh2_SensorValue_t *value);

// bool bno_enableReport(sh2_SensorId_t sensorId, uint32_t interval_us);

esp_err_t bno_start_recording();

// esp_err_t bno_reset();

QueueHandle_t bno_get_result_queue();

void bno_task(void *pvParameters);

#endif