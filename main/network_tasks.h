#pragma once

#include "iebi_types.h"

void tcp_server_task(Queue_task_arg_t *arg);
void tcp_client_task(cameras_iris_src_t *arg);
void http_get_task(cameras_src_t *arg);
void http_post_ds18b20_task(Queue_task_arg_t *arg);
void http_put_ds18b20_task(Queue_task_arg_t *arg);
void http_put_light_sensor_task(int *luz);
