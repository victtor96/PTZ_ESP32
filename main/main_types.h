#pragma once

#include <stdint.h>

#include "freertos/queue.h"
#include "freertos/semphr.h"

#define PORT 3333

#define EXAMPLE_ONEWIRE_BUS_GPIO    5
#define EXAMPLE_ONEWIRE_MAX_DS18B20 2
#define IP_SERVER "192.168.0.32"
#define NVS_NAMESPACE "storage"

typedef struct {
    QueueHandle_t QRF, QRE;
    SemaphoreHandle_t mutex;
} Queue_task_arg_t;

typedef struct {
    char server_IP[20];
    uint8_t gain;
} cameras_src_t;

typedef struct {
    char server_IP[20];
    int server_PORT;
    uint8_t iris[9];
} cameras_iris_src_t;

typedef struct {
    int p_sock;
    SemaphoreHandle_t mutex;
} mutex_sock_src_t;

extern cameras_iris_src_t arg_iris_on[];
extern cameras_iris_src_t arg_iris_off[];
extern cameras_src_t arg_gain_on[];
extern cameras_src_t arg_gain_off[];

extern float ean_temperature;
extern int ean_luzes[15];
