# RMR Material Hazard / Chemical-Evidence Custody Profile V1

**State:** `CUSTODY_PROFILE / CLAIM_GATED`

## Purpose

Apply RMR/BLAKE3 custody to evidence concerning inks, toners, printing particles, solvents, cleaning products, SDS/FDS records, occupational sampling and laboratory reports.

## Boundary

```text
BLAKE3 -> artifact integrity
RMR -> provenance + environment + transformation + custody
SDS/FDS -> product hazard communication
industrial hygiene/toxicology -> exposure/risk interpretation
LGPD -> personal-data governance when persons are identifiable
```

No layer substitutes for another.

## Minimal manifest

```json
{
  "case_id": "TOKEN_VAZIO",
  "artifact_id": "TOKEN_VAZIO",
  "artifact_type": "sds|label|photo|sampling_record|instrument_log|lab_report|statement|metadata",
  "product": {
    "manufacturer": "TOKEN_VAZIO",
    "name": "TOKEN_VAZIO",
    "model_or_cartridge": "TOKEN_VAZIO",
    "lot": "TOKEN_VAZIO"
  },
  "source": {
    "url_or_origin": "TOKEN_VAZIO",
    "captured_at": "TOKEN_VAZIO",
    "method": "TOKEN_VAZIO"
  },
  "integrity": {
    "sha256": "TOKEN_VAZIO",
    "blake3": "TOKEN_VAZIO"
  },
  "privacy": {
    "pii_present": "TOKEN_VAZIO",
    "health_data_present": "TOKEN_VAZIO",
    "public_derivative": "TOKEN_VAZIO"
  },
  "analysis": {
    "expert_review": "TOKEN_VAZIO",
    "exposure_route": "TOKEN_VAZIO",
    "dose_or_concentration": "TOKEN_VAZIO",
    "claim_state": "TOKEN_VAZIO"
  }
}
```

## Claim gates

- `digest_match=true` permits an integrity statement only.
- Product hazard claims require product-specific SDS/FDS or equivalent authoritative evidence.
- Exposure claims require context, route, concentration/dose where applicable, and method.
- Causality claims require appropriate expert/scientific/legal evidence.
- "Fatal", "carcinogenic", "non-toxic" and "safe" are not generic toner/ink labels.
- Unknown product/lots/exposure remain `TOKEN_VAZIO`.

## Privacy

Exposure/health records linked to a natural person can contain sensitive personal data. Preserve the minimum public artifact needed, separate raw protected evidence, and hash the raw artifact when lawful and useful without publishing it.

## External authority pointers

- Brazil LGPD: https://www.planalto.gov.br/ccivil_03/_ato2015-2018/2018/lei/l13709compilado.htm
- Brazil NR-26: https://www.gov.br/trabalho-e-emprego/pt-br/acesso-a-informacao/participacao-social/conselhos-e-orgaos-colegiados/comissao-tripartite-partitaria-permanente/normas-regulamentadora/normas-regulamentadoras-vigentes/norma-regulamentadora-no-26-nr-26
- NIOSH printing controls: https://www.cdc.gov/niosh/engcontrols/ecd/detail37.html
- IARC printing inks/carbon black: https://publications.iarc.who.int/Book-And-Report-Series/Iarc-Monographs-On-The-Identification-Of-Carcinogenic-Hazards-To-Humans/Printing-Processes-And-Printing-Inks-Carbon-Black-And-Some-Nitro-Compounds-1996

## R3

F_ok=physical/chemical evidence can be preserved reproducibly without overstating what a hash proves.  
F_gap=case-specific composition/exposure/causality remains external to BLAKE3.  
F_next=attach only real SDS/FDS or measured evidence under an authorized case manifest.
