# Arquitetura do Firmware

## Objetivo

Controlar parametros de cameras IP (iris/gain) com base em:
- comandos remotos via TCP,
- estado de luminosidade local,
- e telemetria de temperatura.

## Visao de alto nivel

```text
[DS18B20] ---> [task_ds18b20] ---> [HTTP POST/PUT temperatura]

[ADC Luz] ---> [light_sensor] ---> [TCP/HTTP controle cameras]

[TCP Cliente Externo] ---> [tcp_server_task] ---> [task_control_delete]
                                         |--> atualiza presets
                                         |--> salva em NVS
```

## Modulos

### `main.c`
- Ponto de entrada (`app_main`).
- Inicializacao de filas, mutex e criacao de tasks.

### `wifi_manager.c`
- Inicializacao de rede Wi-Fi STA.
- Callback de eventos (`WIFI_EVENT` e `IP_EVENT`).

### `sensor_tasks.c`
- `task_ds18b20`: leitura periodica do sensor DS18B20.
- `light_sensor`: leitura ADC e decisao de perfis de controle.

### `network_tasks.c`
- Servidor TCP na porta `3333`.
- Cliente TCP para comandos de iris.
- Cliente HTTP para endpoints de luz/temperatura e ganho.

### `control_tasks.c`
- Decodificacao do payload recebido no servidor TCP.
- Atualizacao dos perfis `arg_iris_*` e `arg_gain_*`.
- Disparo de chamadas de controle e persistencia.

### `nvs_tasks.c`
- Leitura inicial de presets salvos.
- Persistencia periodica de presets de iris/gain.

### `adc_main.c`
- Configuracao de canal ADC.
- Inicializacao da calibracao de leitura.

### `main_types.h` e `main_globals.c`
- Tipos compartilhados entre modulos.
- Variaveis globais de estado e arrays de presets.

## Concorrencia e sincronizacao

- Cada funcionalidade principal roda em task dedicada (FreeRTOS).
- Mutex usado para serializar alteracoes de estado critico de controle.
- Filas usadas para troca de ponteiros/dados entre task de aquisicao e envio HTTP.

## Persistencia

- Namespace NVS: `storage`.
- Chaves atualmente usadas:
  - `ptz iris on`
  - `ptz iris off`
  - `ptz gain on`
  - `ptz gain off`

## Riscos tecnicos atuais

- Credenciais Wi-Fi e IPs fixos em codigo.
- Controle TCP sem autenticacao.
- Endpoints HTTP e IDs hardcoded.

Esses pontos devem ser priorizados antes de uso em producao.
