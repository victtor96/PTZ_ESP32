# Contribuindo

Obrigado por contribuir com o firmware PTZ_ESP32.

## Fluxo recomendado

1. Abra uma issue descrevendo bug, melhoria ou nova feature.
2. Crie uma branch a partir da `main`:
   ```bash
   git checkout -b feat/nome-curto
   ```
3. Faca commits pequenos e objetivos.
4. Abra um Pull Request com contexto tecnico, riscos e evidencias de teste.

## Padroes de codigo

- Linguagem principal: C (ESP-IDF).
- Preserve comportamento existente em refatoracoes estruturais.
- Evite mudancas amplas sem necessidade funcional.
- Mantenha nomes de funcoes e responsabilidades coesas por modulo.

## Checklist minimo para PR

- Build local concluido com ESP-IDF.
- Sem warnings criticos novos.
- Documentacao atualizada quando houver mudanca de comportamento.
- Mudancas em protocolo, NVS ou rede com nota explicita no PR.

## Commits

Prefira mensagens diretas, por exemplo:
- `feat: adiciona validacao de payload TCP`
- `fix: corrige reconexao em perda de Wi-Fi`
- `docs: atualiza guia de deploy`

## Como reportar bugs

Inclua:
- contexto do hardware,
- versao do ESP-IDF,
- logs relevantes,
- passos para reproduzir,
- comportamento esperado x observado.
