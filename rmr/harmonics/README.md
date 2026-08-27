<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Harmonic Points V1 — geometria, composição, ZIP e entropia

## Escopo

Esta trilha contém uma composição algorítmica original baseada em razões harmônicas matemáticas e pontos geométricos discretos. Não incorpora melodia, gravação, sample ou arranjo de terceiro.

A relação com CientiEspiritual/geometria sagrada é **simbólica e autoral na seleção, organização, texto, código e composição expressiva**. Razões numéricas, formas geométricas, ideias, métodos e conceitos matemáticos em si não são apropriados como exclusividade autoral por este módulo.

## Berna: proteção, não licença

A Convenção de Berna é uma moldura internacional de proteção de obras autorais; ela não é uma licença de software ou de música. A licença do código desta trilha continua sendo `rmr/LICENSE_RMR`, sem alteração automática de termos.

Para a camada RMR:

- copyright/autoria da expressão original pode ser preservado conforme a lei aplicável;
- upstream BLAKE3 mantém autoria e licenças upstream;
- matemática/razões harmônicas não são reivindicadas como propriedade exclusiva;
- obra musical de terceiro não deve ser incorporada sem licença/base jurídica verificável;
- hash, ZIP e receipt provam integridade/custódia limitada, não autoria por si sós.

## Pontos harmônicos

A implementação `rmr_harmonic_points.c` usa oito razões de afinação racional:

`1/1, 9/8, 5/4, 4/3, 3/2, 5/3, 15/8, 2/1`

Cada razão é associada a um ponto de um ciclo de oito posições e a uma fase geométrica. A sequência de 16 passos é uma composição algorítmica original deste módulo, não uma transcrição de música existente.

O gerador produz:

1. `rmr_harmonic_points.wav` — PCM mono 16-bit / 48 kHz;
2. `rmr_harmonic_points.manifest.json` — parâmetros e métrica de entropia;
3. stderr com entropia Shannon dos bytes PCM.

Exemplo:

```sh
cc rmr/harmonics/rmr_harmonic_points.c -O2 -lm -o /tmp/rmr-harmonic-points
/tmp/rmr-harmonic-points /tmp/points.wav /tmp/points.manifest.json 220
```

## Entropia: é boa?

**Como métrica, sim; como objetivo absoluto, não.**

A entropia Shannon ajuda a medir distribuição/informação dos bytes gerados. Entropia maior pode indicar menor regularidade/compressibilidade; entropia menor pode indicar mais repetição/estrutura. Nenhuma das duas é automaticamente “melhor música”, “mais sagrada” ou evidência espiritual.

Portanto:

`ENTROPIA = DIAGNÓSTICO`

`ENTROPIA != QUALIDADE_MUSICAL`

`ENTROPIA != PROVA_CIENTIESPIRITUAL`

A implementação apenas mede e registra; não otimiza cegamente para máximo ou mínimo.

## ZIP

Não é criado um segundo formato ZIP concorrente. Os artefatos desta trilha devem reutilizar a superfície já existente:

- `rmr/crypto/ZIP_BITSTACK_CUSTODY_PROFILE.md`;
- `rmr/crypto/registry/zip_custody_profile.json`;
- `rmr/crypto/tools/validate_zip_custody_profile.py`;
- `rmr/freestanding_custody16/src/rmr_fc16_zip.c` quando aplicável.

O ZIP funciona como **envelope/cápsula de artefatos**. Ele não é prova autônoma de autenticidade. A cadeia correta é:

`WAV + MANIFEST → HASH/DIGEST → ZIP/CAPSULE → RECEIPT → PROVENANCE`

## Gates

`MÚSICA_ORIGINAL != MÚSICA_DE_TERCEIRO`

`MATEMÁTICA != EXPRESSÃO_AUTORAL`

`BERNA != LICENÇA`

`ZIP != AUTORIA`

`HASH != AUTORIA`

`GEOMETRIA_SAGRADA_SIMBÓLICA != RESULTADO_EMPÍRICO`

`claim_allowed=false`

## Próxima validação

- compilar e executar em runner/Termux observável;
- registrar SHA-256/BLAKE3 dos WAV/manifest gerados;
- empacotar pelo perfil ZIP de custódia existente;
- guardar receipt com compiler/flags/exit code/digests;
- comparar entropia apenas como métrica, sem promover claim acústico, terapêutico ou espiritual.
