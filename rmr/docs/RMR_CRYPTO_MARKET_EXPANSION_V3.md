<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Crypto Market Expansion V3

V3 promotes five fixed-output digest profiles from fail-closed backlog to
provider-backed runtime surfaces:

- SHA-512/224;
- SHA-512/256;
- SHA3-224;
- SHA3-384;
- SM3.

Provider authority is OpenSSL 3 EVP.

Standards boundaries:
- SHA-512/224 and SHA-512/256: SHA-2 / FIPS 180-4 family;
- SHA3-224 and SHA3-384: SHA-3 / FIPS 202 family;
- SM3: GB/T 32905-2016 / GM/T 0004-2012 interoperability profile.

All five receive deterministic "abc" KATs. The digest vectors were independently
cross-checked before being committed.

The registry grows from 23 to 28 algorithms. With 9 architecture profiles, the
Cartesian registration target becomes 252 cells. This does not create 252
physical execution receipts: non-native provider execution remains TOKEN_VAZIO.

SM3 classification is deliberately `standards_interoperability`, and the
validator rejects silent promotion to a broader security-use label.

`SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM`
