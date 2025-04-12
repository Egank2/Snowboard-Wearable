#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
// #include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "bno08x.h"

#include "sh2.h"
#include "sh2_SensorValue.h"
#include "sh2_err.h"


#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define INT_PIN GPIO_NUM_32
#define RST_PIN GPIO_NUM_33
#define RST_BITMASK (1ULL << GPIO_NUM_33)

#define I2C_TIMEOUT_MS 1000

#define I2C_MAX_BUF_LEN 250

#define BNO_ADDR 0x4A

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define SQ(x) (x * x)

#define min(a, b) a < b ? a : b

typedef struct {
    double yaw;
    double pitch;
    double roll;
} euler_t;

static const char *TAG = "BNO08X";

static bool _reset_occurred = false;
static sh2_SensorValue_t *_sensor_value = NULL;
static sh2_Hal_t _HAL;
static sh2_ProductIds_t prodIds;
QueueHandle_t result_queue = NULL;
bool recorder_state = false;

gpio_config_t rst_config = {
    .pin_bit_mask = RST_BITMASK,
    .intr_type = GPIO_INTR_DISABLE,    
    .mode = GPIO_MODE_OUTPUT,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE
};


static esp_err_t bno_reset();
static bool bno_enableReport(sh2_SensorId_t sensorId, uint32_t interval_us);

static void hal_callback(void *cookie, sh2_AsyncEvent_t *pEvent);
static void sensorHandler(void *cookie, sh2_SensorEvent_t *event);
static uint32_t hal_getTimeUs(sh2_Hal_t *self);
static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

static int i2chal_open(sh2_Hal_t *self);
static void i2chal_close(sh2_Hal_t *self);
static int i2chal_read(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len, uint32_t *t_us);
static int i2chal_write(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len);

// I hate espressif
// static esp_err_t i2c_master_init_legacy(void) 
// {
//     return ESP_OK;
// }
// esp_err_t i2c_write_legacy(uint8_t slv_address, const uint8_t *data, uint8_t data_len)
// {
//     return i2c_master_write_to_device(0, slv_address, data, data_len * sizeof(uint8_t), pdMS_TO_TICKS(39999999000));
// }

// esp_err_t i2c_read_legacy(uint8_t slv_address, uint8_t *data, uint8_t data_len)
// {
//     return i2c_master_read_from_device(0, slv_address, data, data_len * sizeof(uint8_t), pdMS_TO_TICKS(1000999999999990));
// }

static esp_err_t i2c_master_init(void)
{
    esp_err_t ret;

    if (1 || bus_handle == NULL) {
        i2c_master_bus_config_t i2c_mst_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = I2C_NUM_0,
            .scl_io_num = GPIO_NUM_22,
            .sda_io_num = GPIO_NUM_21,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true
        };

        if (bus_handle != NULL) {
            i2c_master_bus_reset(bus_handle);
            i2c_master_bus_rm_device(dev_handle);
            dev_handle = NULL;
            i2c_del_master_bus(bus_handle);
            for (int i = 0; i < 9; i++) {
                gpio_set_level(i2c_mst_config.scl_io_num, 0);
                vTaskDelay(pdMS_TO_TICKS(1));
                gpio_set_level(i2c_mst_config.scl_io_num, 1);
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        }
        
        if ((ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle)) != ESP_OK) {
            ESP_LOGE(TAG, "Couldn't create master bus: (%s)", esp_err_to_name(ret));
            return ret;
        }
    } else {
        if ((ret = i2c_master_bus_reset(bus_handle)) != ESP_OK) {
            ESP_LOGE(TAG, "Couldn't reset master bus (%s)", esp_err_to_name(ret));
            return ret;
        }   
    }
    
    if (dev_handle == NULL) {
        i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = BNO_ADDR,
            .scl_speed_hz = 10000, // BNO086 needs to be clock stretched and ESP32 doesn't support it so reduce the clock speed to 10kHz
        };
        
        if ((ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle)) != ESP_OK) {
            ESP_LOGE(TAG, "Couldn't add I2C device: (%s)", esp_err_to_name(ret));
            return ret;
        }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_LOGI(TAG, "I2C initialized");
    return ESP_OK;
}

esp_err_t i2c_write(uint8_t slv_address, const uint8_t *data, uint8_t data_len)
{
    return i2c_master_transmit(dev_handle, data, data_len, 1000);
}

esp_err_t i2c_read(uint8_t slv_address, uint8_t *data, uint8_t data_len)
{
    return i2c_master_receive(dev_handle, data, data_len, 1000);
}

void hintn_callback(void* arg)
{
    ESP_LOGI(TAG, "hintn_callback");
}

void quaternionToEuler(double qr, double qi, double qj, double qk, euler_t* ypr, bool degrees) {
    // ESP_LOGI(SPP_TAG, "Before sqrt %fw, %fi, %fj, %fk", qr, qi, qj, qk);
    double sqr = SQ(qr);
    double sqi = SQ(qi);
    double sqj = SQ(qj);
    double sqk = SQ(qk);
    // ESP_LOGI(SPP_TAG, "After sqrt %fw, %fi, %fj, %fk", sqr, sqi, sqj, sqk);
    // ESP_LOGI(SPP_TAG, "Yaw First part: %f, second part: %f", 2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
    ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
    ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
    ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));
    // ESP_LOGI(SPP_TAG, "Rad: yaw = %f, pitch = %f, roll = %f", ypr->yaw, ypr->pitch, ypr->roll);
    if (degrees) {
      ypr->yaw *= RAD_TO_DEG;
      ypr->pitch *= RAD_TO_DEG;
      ypr->roll *= RAD_TO_DEG;
    }
}

void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr, bool degrees) {
    quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

esp_err_t bno_init()
{
    // int i2c_master_port = I2C_MASTER_NUM;

    // i2c_config_t conf = {
    //     .mode = I2C_MODE_MASTER,
    //     .sda_io_num = 21,
    //     .scl_io_num = 22,
    //     .sda_pullup_en = 0,
    //     .scl_pullup_en = 0,
    //     .master.clk_speed = I2C_MASTER_FREQ_HZ,
    // };

    // i2c_param_config(i2c_master_port, &conf);

    // i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    esp_err_t ret = gpio_config(&rst_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't configure RST GPIO! (%s)", esp_err_to_name(ret));
    }

    result_queue = xQueueCreate(5, sizeof(quat_t));

    if ((ret = bno_reset()) != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't reset IMU, continuing (%s)", esp_err_to_name(ret));
    }

    if ((ret = i2c_master_init()) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init I2C (%s)", esp_err_to_name(ret));
        return ret;
    }

    _HAL.open = i2chal_open;
    _HAL.close = i2chal_close;
    _HAL.read = i2chal_read;
    _HAL.write = i2chal_write;
    _HAL.getTimeUs = hal_getTimeUs;

    // Open SH2, if it fails then sh2 may already be open, so try closing then re-open
    int status = sh2_open(&_HAL, hal_callback, NULL);
    if (status != SH2_OK) {
        ESP_LOGW(TAG, "Couldn't open sh2");
        sh2_reinitialize();
        // ESP_LOGI(TAG, "SH2 may already open, closing and reopening... (%d)", status);
        // status = sh2_open(&_HAL, hal_callback, NULL);
        // if (status != SH2_OK) {
        //     ESP_LOGE(TAG, "Couldn't open sh2 (%d)", status);
        //     return ESP_FAIL;
        // }
    }

    memset(&prodIds, 0, sizeof(prodIds));
    status = sh2_getProdIds(&prodIds);
    if (status != SH2_OK) {
        ESP_LOGE(TAG, "Couldn't get product ids (%d)", status);
        // return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Part number: %lu", prodIds.entry[0].swPartNumber);

    sh2_setSensorCallback(sensorHandler, NULL);
    
    ESP_LOGI(TAG, "BNO Initialized");
    sh2_SensorId_t reportType = SH2_ROTATION_VECTOR;
    bno_enableReport(reportType, 30769); // ~32.5Hz update rate
    ESP_LOGI(TAG, "Reports enabled");

    return ESP_OK;
}

bool bno_getSensorEvent(sh2_SensorValue_t *value) 
{
    _sensor_value = value;

    value->timestamp = 0;

    sh2_service();

    if (value->timestamp == 0 && value->sensorId != SH2_GYRO_INTEGRATED_RV) {
        // no new events
        return false;
    }

    return true;
}

bool bno_enableReport(sh2_SensorId_t sensorId, uint32_t interval_us) 
{
    static sh2_SensorConfig_t config;

    // These sensor options are disabled or not used in most cases
    config.changeSensitivityEnabled = false;
    config.wakeupEnabled = false;
    config.changeSensitivityRelative = false;
    config.alwaysOnEnabled = false;
    config.changeSensitivity = 0;
    config.batchInterval_us = 0;
    config.sensorSpecific = 0;

    config.reportInterval_us = interval_us;
    
    int status = sh2_setSensorConfig(sensorId, &config);

    if (status != SH2_OK) {
        ESP_LOGE(TAG, "Couldn't set sensor config (%d)", status);
        return false;
    }

    return true;
}

esp_err_t bno_start_recording() 
{
    recorder_state = true;
    return ESP_OK;
}

esp_err_t bno_reset() 
{
    esp_err_t ret = gpio_set_level(RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    ret = gpio_set_level(RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    return ret;
}

QueueHandle_t bno_get_result_queue() {
    return result_queue;
}

void bno_task(void *pvParameters)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(bno_init());

    sh2_SensorValue_t value;
    euler_t ypr;
    uint64_t eit = 0, it = 2;
    esp_err_t ret;
    quat_t quat_result;
    
    while (1) {
        if (bno_getSensorEvent(&value)) {
            if (it % 900 == 0) {
                UBaseType_t stack_size = uxTaskGetStackHighWaterMark(NULL);
                ESP_LOGD(TAG, "Stack size: %lu", stack_size * sizeof(configSTACK_DEPTH_TYPE));
            }

            // Get latest IMU result
            switch (value.sensorId) {
                case SH2_ROTATION_VECTOR:
                {
                    quaternionToEulerRV(&value.un.rotationVector, &ypr, true);

                    sh2_RotationVectorWAcc_t *rotvec = &value.un.rotationVector;
                    quat_result.w = rotvec->real;
                    quat_result.x = rotvec->i;
                    quat_result.y = rotvec->j;
                    quat_result.z = rotvec->k;
                    BaseType_t rtos_ret;
                    
                    // Queue quaternion
                    if ((rtos_ret = xQueueSend(result_queue, &quat_result, 0)) == errQUEUE_FULL) {
                        // If queue is full, then pop the last measurement
                        quat_t temp;
                        xQueueReceive(result_queue, &temp, 0);
                        xQueueSend(result_queue, &quat_result, 0);
                    } else if (rtos_ret != pdTRUE) {
                        ESP_LOGE(TAG, "Couldn't queue quat result");
                    }
                    
                    if (it++ % 300 == 0) {
                            ESP_LOGI(TAG, "yaw = %.1f, pitch = %.1f, roll = %.1f", ypr.yaw, ypr.pitch, ypr.roll);
                    }       
                } 
                    break;
                default:
                    ESP_LOGW(TAG, "Other sensor event: %u", value.sensorId);
            }
            eit = 1;
            if (it++ % 800 == 0) {
                i2c_master_init();
                it++;
            }    
        } else if (eit++ >= 1000) { 
            // ESP_LOGW(TAG, "No sensor events, restarting");
            // if ((ret = bno_reset()) != ESP_OK) {
            //     ESP_LOGE(TAG, "Couldn't reset bno (%s)", esp_err_to_name(ret));
            // } else {
            //     if ((ret = bno_init()) != ESP_OK) {
            //         ESP_LOGE(TAG, "Couldn't re-initialize bno (%s)", esp_err_to_name(ret));
            //     } else {
            //         eit = 0;
            //     }
            // }
        }
    
        if (recorder_state) {

        }
    }

    vTaskDelete(NULL);
}

esp_err_t bno_tareXY() 
{
    uint8_t ret = sh2_setTareNow(SH2_TARE_X | SH2_TARE_Y, SH2_TARE_BASIS_ROTATION_VECTOR);
    if (ret != SH2_OK) {
        ESP_LOGE(TAG, "Failed to set tare: %d", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static uint32_t hal_getTimeUs(sh2_Hal_t *self) {
    return esp_timer_get_time();
}

static void hal_callback(void *cookie, sh2_AsyncEvent_t *pEvent) 
{
    // If we see a reset, set a flag so that sensors will be reconfigured.
    if (pEvent->eventId == SH2_RESET) {
        ESP_LOGW(TAG, "SH2 reset occured");
        _reset_occurred = true;
    }
}

static void sensorHandler(void *cookie, sh2_SensorEvent_t *event) 
{
    int rc;

    rc = sh2_decodeSensorEvent(_sensor_value, event);
    if (rc != SH2_OK) {
        ESP_LOGE(TAG, "Error decoding sensor event (%d)", rc);
        _sensor_value->timestamp = 0;
        return;
    }
}

static int i2chal_open(sh2_Hal_t *self) {
    ESP_LOGD(TAG, "Opening SH2 I2C HAL");
    uint8_t softreset_pkt[] = {5, 0, 1, 0, 1};

    bool success = false;
    esp_err_t ret;
    for (uint8_t attempts = 0; attempts < 5; attempts++) {
        if ((ret = i2c_write(BNO_ADDR, softreset_pkt, sizeof(softreset_pkt) / sizeof(uint8_t))) == ESP_OK) {
            success = true;
            break;
        } else {
            ESP_LOGW(TAG, "Couldn't write soft reset (%s)", esp_err_to_name(ret));
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }

    if (!success)
        return -1;

    vTaskDelay(300 / portTICK_PERIOD_MS);
    return 0;
}

static void i2chal_close(sh2_Hal_t *self) {
    ESP_LOGI(TAG, "SH2 HAL Closed CB");
}

static int i2chal_read(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len, uint32_t *t_us) {
    uint8_t header[4];
    esp_err_t ret; 
    uint32_t retries = 0;

    while ((ret = i2c_read(BNO_ADDR, header, 4)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read SH2 header (%s)", esp_err_to_name(ret));

        if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGI(TAG, "Re-initializing I2C... (%s)", esp_err_to_name(ret));
            gpio_set_level(GPIO_NUM_22, 1);
            gpio_set_level(GPIO_NUM_21, 1);
            ret = i2c_master_init();
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Couldn't reset I2C (%s)", esp_err_to_name(ret));
                return 0;
            } else {
                if (retries > 5) {
                    ESP_LOGE(TAG, "Couldn't read header after retrying");
                    return 0;
                }

                ++retries;
                continue;
            }
        }

        return 0;
    }

    // Determine amount to read
    uint16_t packet_size = (uint16_t)header[0] | (uint16_t)header[1] << 8;
    // Unset the "continue" bit
    packet_size &= ~0x8000;

    /*
    Serial.print("Read SHTP header. ");
    Serial.print("Packet size: ");
    Serial.print(packet_size);
    Serial.print(" & buffer size: ");
    Serial.println(len);
    */

    size_t i2c_buffer_max = I2C_MAX_BUF_LEN;

    if (packet_size > len) {
        // packet wouldn't fit in our buffer
        return 0;
    }
    // the number of non-header bytes to read
    uint16_t cargo_remaining = packet_size;
    uint8_t i2c_buffer[i2c_buffer_max];
    uint16_t read_size;
    uint16_t cargo_read_amount = 0;
    bool first_read = true;
    
    while (cargo_remaining > 0) {
        if (first_read) {
            read_size = min(i2c_buffer_max, (size_t)cargo_remaining);
        } else {
            read_size = min(i2c_buffer_max, (size_t)cargo_remaining + 4);
        }

        // Serial.print("Reading from I2C: "); Serial.println(read_size);
        // Serial.print("Remaining to read: "); Serial.println(cargo_remaining);

        // Try to read header. If I2C is an a invalid state then try to restart it and continue
        if ((ret = i2c_read(BNO_ADDR, i2c_buffer, read_size)) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read SH2 packet (%s)", esp_err_to_name(ret));
            return 0;
        }

        if (first_read) {
            // The first time we're saving the "original" header, so include it in the
            // cargo count
            cargo_read_amount = read_size;
            memcpy(pBuffer, i2c_buffer, cargo_read_amount);
            first_read = false;
        } else {
            // this is not the first read, so copy from 4 bytes after the beginning of
            // the i2c buffer to skip the header included with every new i2c read and
            // don't include the header in the amount of cargo read
            cargo_read_amount = read_size - 4;
            memcpy(pBuffer, i2c_buffer + 4, cargo_read_amount);
        }
        // advance our pointer by the amount of cargo read
        pBuffer += cargo_read_amount;
        // mark the cargo as received
        cargo_remaining -= cargo_read_amount;
    }

    /*
    for (int i=0; i<packet_size; i++) {
    Serial.print(pBufferOrig[i], HEX);
    Serial.print(", ");
    if (i % 16 == 15) Serial.println();
    }
    Serial.println();
    */

    return packet_size;
}

static int i2chal_write(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len) {
    size_t i2c_buffer_max = I2C_MAX_BUF_LEN;

    /*
    Serial.print("I2C HAL write packet size: ");
    Serial.print(len);
    Serial.print(" & max buffer size: ");
    Serial.println(i2c_buffer_max);
    */
  
    uint16_t write_size = min(i2c_buffer_max, len);
    esp_err_t ret;
    if ((ret = i2c_write(BNO_ADDR, pBuffer, write_size)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write i2c: (%s)", esp_err_to_name(ret));
        return 0;
    }
  
    return write_size;
}

/*
3/16/2025 8:04PM

I (4218537) BNO08X: yaw = 62.5, pitch = 5.4, roll = 0.4
E (4218647) i2c.master: I2C hardware timeout detected
E (4218647) i2c.master: s_i2c_synchronous_transaction(924): I2C transaction failed
E (4218647) i2c.master: i2c_master_receive(1247): I2C transaction failed

***ERROR*** A stack overflow in task IMU Recorder has been detected.


Backtrace: 0x40081e1d:0x3ffd0e10 0x4008f9c9:0x3ffd0e30 0x40090832:0x3ffd0e50 0x40091b73:0x3ffd0ed0 0x4009093c:0x3ffd0ef0 0x400908ee:0x3ffd0fe0 0x00060e20:0x3ffd1020 |<-CORRUPTED
--- 0x40081e1d: panic_abort at /home/foamstein/esp/esp-idf/components/esp_system/panic.c:454
0x4008f9c9: esp_system_abort at /home/foamstein/esp/esp-idf/components/esp_system/port/esp_system_chip.c:92
0x40090832: vApplicationStackOverflowHook at /home/foamstein/esp/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:563
0x40091b73: vTaskSwitchContext at /home/foamstein/esp/esp-idf/components/freertos/FreeRTOS-Kernel/tasks.c:3710 (discriminator 7)
0x4009093c: _frxt_dispatch at /home/foamstein/esp/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/portasm.S:451
0x400908ee: _frxt_int_exit at /home/foamstein/esp/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/portasm.S:246
*/