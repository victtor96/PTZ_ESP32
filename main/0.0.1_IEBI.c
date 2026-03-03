#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "iebi_types.h"
#include "wifi_manager.h"
#include "network_tasks.h"
#include "sensor_tasks.h"
#include "nvs_tasks.h"

void app_main(void)
{
    SemaphoreHandle_t mutex_A = xSemaphoreCreateMutex();

    QueueHandle_t QRF_TM = xQueueCreate(2, sizeof(ean_temperature));
    QueueHandle_t QRE_TM = xQueueCreate(2, sizeof(ean_temperature));

    QueueHandle_t QRF_LZ = xQueueCreate(15, sizeof(ean_luzes));
    QueueHandle_t QRE_LZ = xQueueCreate(15, sizeof(ean_luzes));

    float *p_tmp = &ean_temperature;

    for (int i = 0; i < 15; i++) {
        int *plz = ean_luzes;
        xQueueSend(QRE_LZ, &(plz), portMAX_DELAY);
    }

    xQueueSend(QRE_TM, &(p_tmp), portMAX_DELAY);

    Queue_task_arg_t Temp_task_queue_arg[3];

    for (int i = 0; i < 3; i++) {
        Temp_task_queue_arg[i].QRE = QRE_TM;
        Temp_task_queue_arg[i].QRF = QRF_TM;
    }

    Queue_task_arg_t Light_task_queue_arg[3];

    for (int i = 0; i < 3; i++) {
        Light_task_queue_arg[i].QRE = QRE_LZ;
        Light_task_queue_arg[i].QRF = QRF_LZ;
        Light_task_queue_arg[i].mutex = mutex_A;
    }

    wifi_connection();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Function_NVS_iris();
    xTaskCreate((TaskFunction_t)tcp_server_task, "tcp_server", 4096, &Light_task_queue_arg[1], 23, NULL);

    xTaskCreate((TaskFunction_t)light_sensor, "adc_task", 4096 * 2, &Light_task_queue_arg[2], 20, NULL);

    xTaskCreate((TaskFunction_t)task_ds18b20, "ds18b20_task", 4096, &Temp_task_queue_arg[0], 15, NULL);
    xTaskCreate((TaskFunction_t)http_post_ds18b20_task, "http_post_ds18b20_task", 4096, &Temp_task_queue_arg[1], 10, NULL);
    xTaskCreate((TaskFunction_t)http_put_ds18b20_task, "http_post_ds18b20_task", 4096, &Temp_task_queue_arg[2], 10, NULL);
}
