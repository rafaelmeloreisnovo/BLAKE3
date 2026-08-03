# BLAKE3 — receipt de tentativa de captura

**Receipt ID:** `BLAKE3-AUDIT-EXEC-20260803T153600Z`  
**Estado:** `FAIL_ENVIRONMENT`  
**Claim gate:** `claim_allowed=false`

## Resultado

| Etapa | Resultado |
|---|---|
| validação sintática `bash -n` | `PASS` |
| clone do fork | `FAIL_ENVIRONMENT` |
| mirror completo do upstream | não iniciado |
| captura de refs/objetos/DAG | não realizada |
| execução física Android/Termux | `TOKEN_VAZIO` |
| captura do reflog local do aparelho | `TOKEN_VAZIO` |
| réplica do receipt no Google Drive | `PASS` |

## Falha preservada

```text
fatal: unable to access 'https://github.com/rafaelmeloreisnovo/BLAKE3.git/': Could not resolve host: github.com
exit_code=128
```

O ambiente de execução independente estava isolado de rede/DNS. Este receipt **não** pode ser usado como prova de captura bem-sucedida do mirror.

## Artefato

```text
file=BLAKE3_AUDIT_EXECUTION_20260803T153600Z.tar.gz
size=2941 bytes
sha256=3243d8c3356c04534b7399aff24f354d713b55e7857f804e13ba69a691f5728a
```

## Réplica Google Drive

```text
00_CANONE_E_GOVERNANCA/
└── BLAKE3_AUDIT/
    └── 20260803_EXECUTION_RECEIPTS/
        ├── BLAKE3_AUDIT_EXECUTION_20260803T153600Z.tar.gz
        └── BLAKE3_AUDIT_EXECUTION_20260803T153600Z.tar.gz.sha256
```

- archive: https://drive.google.com/file/d/1eHYrz5d0OzhNOh-fxiIevAyY60_dHB2F/view
- checksum: https://drive.google.com/file/d/1o1641hxZJKmcgYpDh7--Z9EmidxqKbVT/view

## Gate ainda aberto

Executar `tools/forensics/capture_blake3_routes.sh` no aparelho Termux com rede funcional e preservar o primeiro `snapshots/<UTC>/`. Somente esse evento poderá fechar o gate de captura física e reflog local.