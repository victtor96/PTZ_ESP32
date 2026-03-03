# Deploy e Operacao

## 1) Pre-requisitos

- ESP-IDF instalado (recomendado: `v5.4.1`).
- Toolchain para ESP32 configurada.
- Cabo serial e permissao de acesso a porta.

## 2) Configurar ambiente

```bash
. $HOME/esp-idf/export.sh
```

## 3) Build

```bash
idf.py set-target esp32
idf.py build
```

## 4) Flash + monitor

```bash
idf.py -p <PORTA_SERIAL> flash monitor
```

Exemplo:

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## 5) Verificacao inicial

- Confirmar conexao Wi-Fi no log.
- Confirmar IP recebido (`IP_EVENT_STA_GOT_IP`).
- Confirmar inicializacao do barramento 1-Wire.
- Confirmar recebimento de comandos TCP na porta `3333`.

## 6) Parametros operacionais relevantes

- Porta TCP local: `3333`.
- Servidor HTTP alvo: `IP_SERVER`.
- Namespace NVS: `storage`.

## 7) Publicacao no GitHub

Checklist recomendado:
- [ ] Remover credenciais reais de Wi-Fi.
- [ ] Revisar IPs fixos e IDs de endpoint.
- [ ] Validar `.gitignore` antes do primeiro commit.
- [ ] Revisar licenca e metadados de autoria.
- [ ] Subir `README`, docs e templates de issue/PR.
