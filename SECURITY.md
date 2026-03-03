# Politica de Seguranca

## Reporte de vulnerabilidades

Se voce identificar uma vulnerabilidade, abra uma issue privada com:
- descricao tecnica,
- impacto potencial,
- passo a passo para reproducao,
- sugestao de mitigacao (se houver).

Evite publicar exploracoes detalhadas em issues publicas antes da correcao.

## Escopo de seguranca

Este projeto envolve:
- conectividade Wi-Fi,
- comunicacao TCP/HTTP em rede local,
- comandos de controle para dispositivos IP,
- persistencia em NVS.

Pontos de atencao esperados em revisoes:
- validacao de payload de rede,
- tratamento de erro em sockets,
- credenciais em codigo,
- limites de buffer e parsing.

## Boas praticas recomendadas

- Nao versionar credenciais reais em producao.
- Usar segmentacao de rede para dispositivos de controle.
- Avaliar autenticacao para comandos TCP antes de ambiente produtivo.
- Revisar periodicamente bibliotecas e versoes do ESP-IDF.
