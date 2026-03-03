#include "network_tasks.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "control_tasks.h"

static void server_handle_client(mutex_sock_src_t *arg);

void http_put_light_sensor_task(int *luz)
{
    char requets[600];
    char ip[] = IP_SERVER;
    char path[] = "/luztr/67743c4d5fc232db123eeb9b";
    char data[40];

    snprintf(data, sizeof(data), "{\"y\": %d}", *luz);

    struct sockaddr_in dest_addr;

    dest_addr.sin_addr.s_addr = inet_addr(IP_SERVER);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(3000);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    snprintf(requets, sizeof(requets),
             "PUT %s HTTP/1.0\r\n"
             "Host: %s:3000\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             path, ip, (int)strlen(data), data);

    send(sock, requets, sizeof(requets), 0);

    shutdown(sock, 0);
    close(sock);

    vTaskDelete(NULL);
}

void tcp_server_task(Queue_task_arg_t *arg)
{
    struct sockaddr_storage dest_addr;
    /* 2000 */

    mutex_sock_src_t sock_mutex;

    sock_mutex.mutex = arg->mutex;

    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(PORT);

    // Open socket
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0); // 0 for TCP Protocol
    int opt = 1;

    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    // Listen to the socket
    listen(listen_sock, 1);

    while (true) {
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);

        // Accept socket
        sock_mutex.p_sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

        // Cria uma nova task para o cliente
        if (sock_mutex.p_sock > 0) {
            xTaskCreate((TaskFunction_t)server_handle_client, "handle_client", 4096, &sock_mutex, 5, NULL);
        }

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    close(listen_sock);
    vTaskDelete(NULL);
}

static void server_handle_client(mutex_sock_src_t *arg)
{
    int keepAlive = 1;
    int keepIdle = 1;
    int keepInterval = 1;
    int keepCount = 3;
    unsigned char buffer[8] = {0};
    const char *message_error = "Mensagem Invalida";
    const char *message_ok = "Mensagem Valida";
    SemaphoreHandle_t mutex_A = arg->mutex;

    int sock = arg->p_sock;

    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, 8, 0);

    if (buffer[0] != 0x81) {
        send(sock, message_error, strlen(message_error), 0);
        shutdown(sock, 0);
        close(sock);

        vTaskDelay(200 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    } else {
        task_control_delete(buffer, sizeof(buffer), mutex_A);
        send(sock, message_ok, strlen(message_ok), 0);
    }

    printf("\nSERVER CLOSE\n");

    shutdown(sock, 0);
    close(sock);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void tcp_client_task(cameras_iris_src_t *arg)
{
    char local_IP[20];
    strcpy(local_IP, arg->server_IP);
    int local_PORT = arg->server_PORT;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(local_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(local_PORT);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    int ook = send(sock, arg->iris, sizeof(arg->iris), 0);
    printf("sende iris %d\n", ook);

    shutdown(sock, 0);
    close(sock);

    vTaskDelay(200 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void http_put_ds18b20_task(Queue_task_arg_t *arg)
{
    char requets[600];
    char ip[] = IP_SERVER;
    char path[] = "/temprt/67718495ab68eae27e84c34c";
    char data[40];

    float *temp1;
    QueueHandle_t QRF = arg->QRF;
    QueueHandle_t QRE = arg->QRE;

    while (true) {
        while (!xQueueReceive(QRF, &temp1, portMAX_DELAY))
            ;

        snprintf(data, sizeof(data), "{\"y\": %02.f}", *temp1);

        while (!xQueueSend(QRE, (void *)&temp1, portMAX_DELAY))
            ;

        struct sockaddr_in dest_addr;

        dest_addr.sin_addr.s_addr = inet_addr(IP_SERVER);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(3000);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        snprintf(requets, sizeof(requets),
                 "PUT %s HTTP/1.0\r\n"
                 "Host: %s:3000\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %d\r\n"
                 "\r\n"
                 "%s",
                 path, ip, (int)strlen(data), data);

        printf("\nput temp\n");

        send(sock, requets, sizeof(requets), 0);

        shutdown(sock, 0);
        close(sock);
        vTaskDelay(45000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void http_post_ds18b20_task(Queue_task_arg_t *arg)
{
    char requets[600];
    char ip[] = IP_SERVER;
    char path[] = "/temperatura";
    char data[40];

    float *temp1;
    QueueHandle_t QRF = arg->QRF;
    QueueHandle_t QRE = arg->QRE;

    while (true) {
        while (!xQueueReceive(QRF, &temp1, portMAX_DELAY))
            ;

        snprintf(data, sizeof(data), "{\"y\": %02.f}", *temp1);

        while (!xQueueSend(QRE, (void *)&temp1, portMAX_DELAY))
            ;

        printf("\nput temp\n");

        struct sockaddr_in dest_addr;

        dest_addr.sin_addr.s_addr = inet_addr(IP_SERVER);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(3000);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        snprintf(requets, sizeof(requets),
                 "POST %s HTTP/1.0\r\n"
                 "Host: %s:3000\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %d\r\n"
                 "\r\n"
                 "%s",
                 path, ip, (int)strlen(data), data);

        send(sock, requets, sizeof(requets), 0);

        shutdown(sock, 0);
        close(sock);
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

/* http_post_task
static void http_post_task(void *i)
{
    char requets[600];
    char ip[]="192.168.0.13:80";
    char path[]= "/api/save";
    char data[]= "{\"tmp\":\"value\"}";

    struct sockaddr_in dest_addr;

        dest_addr.sin_addr.s_addr = inet_addr(IP_SERVER);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(3000);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

     snprintf(requets, sizeof(requets),
             "POST %s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %d\r\n"
             "\r\n"
             "%s",
             path, ip, (int)strlen(data), data);


    send(sock, requets, sizeof(requets), 0);
    shutdown(sock, 0);
    close(sock);
    vTaskDelay(60000 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);

}
*/

void http_get_task(cameras_src_t *arg)
{
    int i = arg->gain;
    char request[500];
    struct sockaddr_in dest_addr;

    char path1[] = "/ajaxcom?szCmd=\%7B\%22SetEnv\%22\%3A\%7B\%22VideoParam\%22\%3A+\%5B\%7B\%22stExp\%22\%3A+%7B\%22again\%22\%3A";
    char path2[] = "\%7D\%2C\%22nChannel\%22\%3A0\%7D\%5D\%7D\%7D";
    printf("\nIP %s - > GAIN: %d\n", arg->server_IP, arg->gain);

    // Configuracao do endereco do servidor
    dest_addr.sin_addr.s_addr = inet_addr(arg->server_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(80);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    // Formatacao da requisicao GET
    snprintf(request, sizeof(request),
             "GET %s%d%s HTTP/1.0\r\n"
             "Host: %s:80\r\n"
             "User-Agent: esp-idf/1.0 esp32\r\n"
             "\r\n",
             path1, i, path2, arg->server_IP);

    // Enviar requisicao
    send(sock, request, strlen(request), 0);

    shutdown(sock, 0);
    close(sock);
    vTaskDelete(NULL);
}
