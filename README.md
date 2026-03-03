# PTZ_ESP32 Firmware (ESP32)

Firmware embarcado para ESP32 com foco em automacao de iluminacao e controle de cameras IP (iris/gain), incluindo:
- sensor de temperatura DS18B20 (1-Wire),
- leitura de luminosidade via ADC,
- servidor TCP para comandos de controle,
- cliente TCP/HTTP para integracao com servicos externos,
- persistencia de parametros em NVS.

## Visao geral

O projeto foi modularizado para facilitar manutencao e evolucao, mantendo o comportamento funcional original.

### Principais capacidades
- Conexao Wi-Fi STA com reconexao automatica e reinicio em falha persistente.
- Coleta periodica de temperatura (DS18B20).
- Envio de temperatura para API HTTP (`POST` e `PUT`).
- Monitoramento de luz ambiente por ADC e acionamento de perfis de camera.
- Servidor TCP na porta `3333` para receber comandos de controle (8 bytes).
- Gravacao e leitura de presets de iris/gain na NVS.

## Arquitetura

### Estrutura de modulos (`main/`)
- `main.c`: `app_main` e orquestracao de tasks/filas.
- `main_types.h` / `main_globals.c`: tipos compartilhados, macros e estado global.
- `wifi_manager.c`: inicializacao e eventos de Wi-Fi.
- `network_tasks.c`: servidor TCP e clientes TCP/HTTP.
- `sensor_tasks.c`: task de temperatura e task de sensor de luz.
- `control_tasks.c`: decodificacao do protocolo de controle e disparo de acoes.
- `nvs_tasks.c`: persistencia de presets no namespace NVS.
- `adc_main.c` / `adc_main.h`: configuracao de ADC e calibracao.

Documentacao detalhada:
- [Arquitetura](docs/ARCHITECTURE.md)
- [Deploy e operacao](docs/DEPLOYMENT.md)
- [Protocolo TCP de controle](docs/PROTOCOL.md)

## Requisitos

- ESP-IDF `v5.4.1` (compatibilidade indicada em `dependencies.lock`).
- Python 3 e toolchain configurados via ESP-IDF.
- Placa ESP32 suportada pelo projeto.

## Dependencias

Gerenciadas por `idf_component.yml`:
- `espressif/ds18b20`
- dependencia transitiva: `espressif/onewire_bus`

## Build e flash

No diretorio raiz do projeto:

```bash
idf.py set-target esp32
idf.py build
idf.py -p <PORTA_SERIAL> flash monitor
```

Exemplo:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Configuracao

Atencao: atualmente alguns parametros estao fixos em codigo.

- Wi-Fi: `main/wifi_manager.c`
  - `WIFI_SSID`
  - `WIFI_PASS`
- Servidor HTTP destino: `IP_SERVER` em `main/main_types.h`
- Namespace NVS: `NVS_NAMESPACE` em `main/main_types.h`
- Porta servidor TCP: `PORT` em `main/main_types.h`

## Fluxo funcional resumido

1. `app_main` inicializa mutex e filas.
2. Wi-Fi conecta em modo STA.
3. Dados de iris/gain sao carregados da NVS.
4. Tasks principais sao iniciadas:
   - servidor TCP,
   - monitoramento de luz (ADC),
   - aquisicao DS18B20,
   - envio HTTP da temperatura.

## Qualidade e colaboracao

- Guia de contribuicao: [CONTRIBUTING.md](CONTRIBUTING.md)
- Politica de seguranca: [SECURITY.md](SECURITY.md)
- Historico de versoes: [CHANGELOG.md](CHANGELOG.md)

## Licenca

Este projeto esta sob licenca MIT. Consulte [LICENSE](LICENSE).
