#include "nvs_tasks.h"

#include <stdio.h>

#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "iebi_types.h"

static void save_ptz_data_to_nvs_iris(nvs_handle_t my_handle);
static void read_ptz_data_from_nvs_iris(nvs_handle_t my_handle, cameras_iris_src_t *retrieved_data, char title[]);
static void read_ptz_data_from_nvs_gain(nvs_handle_t my_handle, cameras_src_t *retrieved_data, char title[]);

void Function_NVS_iris(void)
{
    // Inicialize NVS
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // Abra o NVS para leitura e escrita
    nvs_handle_t my_handle;
    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
    ESP_ERROR_CHECK(ret);

    // Salve as structs no NVS
    static int x = 1;
    // Leia os dados salvos
    cameras_iris_src_t retrieved_data_iris[3];
    cameras_src_t retrieved_data_gain[3];

    if (x == 1) {
        read_ptz_data_from_nvs_iris(my_handle, &retrieved_data_iris, "ptz iris on");

        arg_iris_on[0].iris[7] = retrieved_data_iris[0].iris[7];
        arg_iris_on[1].iris[7] = retrieved_data_iris[1].iris[7];
        arg_iris_on[2].iris[7] = retrieved_data_iris[2].iris[7];

        read_ptz_data_from_nvs_iris(my_handle, &retrieved_data_iris, "ptz iris off");

        arg_iris_off[0].iris[7] = retrieved_data_iris[0].iris[7];
        arg_iris_off[1].iris[7] = retrieved_data_iris[1].iris[7];
        arg_iris_off[2].iris[7] = retrieved_data_iris[2].iris[7];

        read_ptz_data_from_nvs_gain(my_handle, &retrieved_data_gain, "ptz gain on");
        arg_gain_on[0].gain = retrieved_data_gain[0].gain;
        arg_gain_on[1].gain = retrieved_data_gain[1].gain;
        arg_gain_on[2].gain = retrieved_data_gain[2].gain;

        read_ptz_data_from_nvs_gain(my_handle, &retrieved_data_gain, "ptz gain off");
        arg_gain_off[0].gain = retrieved_data_gain[0].gain;
        arg_gain_off[1].gain = retrieved_data_gain[1].gain;
        arg_gain_off[2].gain = retrieved_data_gain[2].gain;

        printf("\n\nDATA SALVA - %02X - \n\n", retrieved_data_iris[0].iris[7]);
        x = 0;
    }

    save_ptz_data_to_nvs_iris(my_handle);
    // Feche o NVS
    nvs_close(my_handle);
}

static void save_ptz_data_to_nvs_iris(nvs_handle_t my_handle)
{
    // Salve o array de structs como um blob
    esp_err_t ret = nvs_set_blob(my_handle, "ptz iris on", arg_iris_on, sizeof(arg_iris_on));
    ret = nvs_commit(my_handle);
    nvs_set_blob(my_handle, "ptz iris off", arg_iris_off, sizeof(arg_iris_off));
    ret = nvs_commit(my_handle);
    nvs_set_blob(my_handle, "ptz gain on", arg_gain_on, sizeof(arg_gain_on));
    ret = nvs_commit(my_handle);
    nvs_set_blob(my_handle, "ptz gain off", arg_gain_off, sizeof(arg_gain_off));
    ret = nvs_commit(my_handle);

    ESP_ERROR_CHECK(ret);

    printf("PTZ data array saved successfully.");
}

static void read_ptz_data_from_nvs_iris(nvs_handle_t my_handle, cameras_iris_src_t *retrieved_data, char title[])
{
    // Leia o array de structs a partir do NVS
    size_t required_size = sizeof(arg_iris_on);
    esp_err_t ret = nvs_get_blob(my_handle, title, retrieved_data, &required_size);

    if (ret == ESP_OK) {
        printf("PTZ data array retrieved successfully.");
    } else {
        printf("Failed to read PTZ data array from NVS.");
    }
}

static void read_ptz_data_from_nvs_gain(nvs_handle_t my_handle, cameras_src_t *retrieved_data, char title[])
{
    // Leia o array de structs a partir do NVS
    size_t required_size = sizeof(arg_gain_on);
    esp_err_t ret = nvs_get_blob(my_handle, title, retrieved_data, &required_size);

    if (ret == ESP_OK) {
        printf("PTZ data array retrieved successfully.");
    } else {
        printf("Failed to read PTZ data array from NVS.");
    }
}
