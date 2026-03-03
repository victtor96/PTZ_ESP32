#include "sensor_tasks.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"

#include "onewire_bus.h"
#include "ds18b20.h"

#include "adc_main.h"
#include "network_tasks.h"

void task_ds18b20(Queue_task_arg_t *arg)
{
    QueueHandle_t QRF = arg->QRF;
    QueueHandle_t QRE = arg->QRE;

    onewire_bus_handle_t bus;
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = EXAMPLE_ONEWIRE_BUS_GPIO,
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10, // 1byte ROM command + 8byte ROM number + 1byte device command
    };
    onewire_new_bus_rmt(&bus_config, &rmt_config, &bus);
    printf("1-Wire bus installed on GPIO %d", EXAMPLE_ONEWIRE_BUS_GPIO);

    int ds18b20_device_num = 0;
    ds18b20_device_handle_t ds18b20s[EXAMPLE_ONEWIRE_MAX_DS18B20];
    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_onewire_device;
    esp_err_t search_result = ESP_OK;

    // create 1-wire device iterator, which is used for device search
    onewire_new_device_iter(bus, &iter);

    do {
        search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
        if (search_result == ESP_OK) { // found a new device, let's check if we can upgrade it to a DS18B20
            ds18b20_config_t ds_cfg = {};
            if (ds18b20_new_device(&next_onewire_device, &ds_cfg, &ds18b20s[ds18b20_device_num]) == ESP_OK) {
                ds18b20_device_num++;
            }
        }
    } while (search_result != ESP_ERR_NOT_FOUND);

    onewire_del_device_iter(iter);

    // set resolution for all DS18B20s
    for (int i = 0; i < ds18b20_device_num; i++) {
        // set resolution
        ds18b20_set_resolution(ds18b20s[i], DS18B20_RESOLUTION_12B);
    }

    float temperature;
    float *temp;

    while (true) {
        while (!xQueueReceive(QRE, &temp, portMAX_DELAY))
            ;

        for (int i = 0; i < ds18b20_device_num; i++) {
            ds18b20_trigger_temperature_conversion(ds18b20s[i]);
            ds18b20_get_temperature(ds18b20s[i], &temperature);
            temp = &temperature;
        }

        while (!xQueueSend(QRF, &temp, portMAX_DELAY))
            ;

        vTaskDelay(45000 / portTICK_PERIOD_MS);
    }
}

void light_sensor(Queue_task_arg_t *arg)
{
    SemaphoreHandle_t xMutex = arg->mutex;

    static int luz_apagada = 0;

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = EXAMPLE_ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    bool do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, EXAMPLE_ADC1_CHAN0, EXAMPLE_ADC_ATTEN, &adc1_cali_chan0_handle);

    while (true) {
        adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]);

        if (adc_raw[0][0] < 300 && luz_apagada != 1) {
            xSemaphoreTake(xMutex, portMAX_DELAY);

            xTaskCreate((TaskFunction_t)http_put_light_sensor_task, "adc_task_print", 4096, &adc_raw[0][0], 10, NULL);
            printf("LUZ APAGADA\n");
            xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_off[0], 22, NULL);
            xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_off[1], 22, NULL);
            xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_off[2], 22, NULL);

            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[0], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[1], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[2], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[3], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_off[4], 22, NULL);

            luz_apagada = 1;
            xSemaphoreGive(xMutex);
        } else if (adc_raw[0][0] > 400 && luz_apagada != 2) {
            xSemaphoreTake(xMutex, portMAX_DELAY);

            printf("LUZ LIGADA\n");
            xTaskCreate((TaskFunction_t)http_put_light_sensor_task, "adc_task_print", 4096, &adc_raw[0][0], 10, NULL);

            xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_on[0], 22, NULL);
            xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_on[1], 22, NULL);
            xTaskCreate((TaskFunction_t)http_get_task, "http_get_task", 4096, &arg_gain_on[2], 22, NULL);

            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[0], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[1], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[2], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[3], 22, NULL);
            xTaskCreate((TaskFunction_t)tcp_client_task, "tcp_client_task", 4096, &arg_iris_on[4], 22, NULL);

            luz_apagada = 2;
            xSemaphoreGive(xMutex);
        }

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
