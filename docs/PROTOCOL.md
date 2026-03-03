# Protocolo TCP de Controle

## Resumo

O firmware expoe um servidor TCP na porta `3333` e espera payloads de `8 bytes`.

- Se `byte[0] != 0x81`: responde `Mensagem Invalida`.
- Se valido: processa comando e responde `Mensagem Valida`.

## Mapeamento de bytes (implementacao atual)

- `byte[0]`: cabecalho (`0x81` esperado)
- `byte[1]`: tipo de comando
  - `0x22`: requisicao de estado/luz
  - `0x11`: comando de PTZ/parametros
- `byte[2]`: perfil
  - `0x01`: perfil "luz ligada"
  - `0x00`: perfil "luz apagada"
- `byte[3]`: camera
  - `0x01`: camera 1
  - `0x02`: camera 2
  - `0x03`: camera 3
- `byte[4]`: parametro
  - `0x91`: iris
  - `0x92`: gain
  - `0x93`: reservado/nao implementado
- `byte[5]` e `byte[6]`: valor do parametro (uso depende do tipo)
- `byte[7]`: reservado

## Efeito dos comandos

- Atualiza arrays globais de presets (`arg_iris_*`, `arg_gain_*`).
- Dispara task TCP/HTTP para aplicar ajuste nas cameras.
- Persiste o estado em NVS via `Function_NVS_iris()`.

## Observacoes

- O protocolo atual nao possui autenticacao.
- Recomenda-se adicionar validacao de tamanho, origem e assinatura antes de uso em ambiente exposto.
