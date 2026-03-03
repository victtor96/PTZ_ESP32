#pragma once

#include <stdint.h>

#include "freertos/semphr.h"

void task_control_delete(uint8_t *seletor, int lenght, SemaphoreHandle_t mutex_A);
