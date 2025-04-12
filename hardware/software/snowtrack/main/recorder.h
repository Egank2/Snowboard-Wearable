#ifndef _RECORDER_H
#define _RECORDER_H

#include "esp_err.h"

esp_err_t rec_init();

esp_err_t rec_start();

esp_err_t rec_stop();

#endif