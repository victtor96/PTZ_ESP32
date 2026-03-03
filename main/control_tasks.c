#include "control_tasks.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "adc_main.h"
#include "main_types.h"
#include "network_tasks.h"
#include "nvs_tasks.h"

void task_control_delete(uint8_t *seletor, int lenght, SemaphoreHandle_t mutex_A)
{
    SemaphoreHandle_t xMutex = mutex_A;

    printf("\n");

    if (seletor[1] == 0x22) {
        xSemaphoreTake(xMutex, portMAX_DELAY);

        xTaskCreate((TaskFunction_t)http_put_light_sensor_task, "adc_task_print", 4096, &adc_raw[0][0], 10, NULL);

        xSemaphoreGive(xMutex);
    }

    if (seletor[1] == 0x11) {
        xSemaphoreTake(xMutex, portMAX_DELAY);

        if (seletor[2] == 0x01) {
            if (seletor[3] == 0x01) {
                if (seletor[4] == 0x91) {
                    arg_iris_on[0].iris[6] = seletor[5];
                    arg_iris_on[0].iris[7] = seletor[6];
                    xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[0], 22, NULL);
                } else if (seletor[4] == 0x92) {
                    arg_gain_on[0].gain = seletor[6];
                    xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_on[0], 22, NULL);
                } else if (seletor[4] == 0x93) {
                }
            }

            else if (seletor[3] == 0x02) {
                if (seletor[4] == 0x91) {
                    arg_iris_on[1].iris[6] = seletor[5];
                    arg_iris_on[1].iris[7] = seletor[6];
                    xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[1], 22, NULL);
                } else if (seletor[4] == 0x92) {
                    arg_gain_on[1].gain = seletor[6];
                    xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_on[1], 22, NULL);

                } else if (seletor[4] == 0x93) {
                }

            }

            else if (seletor[3] == 0x03) {
                if (seletor[4] == 0x91) {
                    arg_iris_on[2].iris[6] = seletor[5];
                    arg_iris_on[2].iris[7] = seletor[6];
                    xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[2], 22, NULL);

                } else if (seletor[4] == 0x92) {
                    arg_gain_on[2].gain = seletor[6];
                    xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_on[2], 22, NULL);

                } else if (seletor[4] == 0x93) {
                }
            }
        }

        if (seletor[2] == 0x00) {
            if (seletor[3] == 0x01) {
                if (seletor[4] == 0x91) {
                    arg_iris_off[0].iris[6] = seletor[5];
                    arg_iris_off[0].iris[7] = seletor[6];
                    xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[0], 22, NULL);

                } else if (seletor[4] == 0x92) {
                    arg_gain_off[0].gain = seletor[6];
                    xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_off[0], 22, NULL);

                } else if (seletor[4] == 0x93) {
                }

            }

            else if (seletor[3] == 0x02) {
                if (seletor[4] == 0x91) {
                    arg_iris_off[1].iris[6] = seletor[5];
                    arg_iris_off[1].iris[7] = seletor[6];
                    xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[1], 22, NULL);

                } else if (seletor[4] == 0x92) {
                    arg_gain_off[1].gain = seletor[6];
                    xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_off[1], 22, NULL);

                } else if (seletor[4] == 0x93) {
                }

            }

            else if (seletor[3] == 0x03) {
                if (seletor[4] == 0x91) {
                    arg_iris_off[2].iris[6] = seletor[5];
                    arg_iris_off[2].iris[7] = seletor[6];
                    xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[2], 22, NULL);

                } else if (seletor[4] == 0x92) {
                    arg_gain_off[2].gain = seletor[6];
                    xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_off[2], 22, NULL);

                } else if (seletor[4] == 0x93) {
                }

            }
        }
        Function_NVS_iris();
        xSemaphoreGive(xMutex);
    }
}
