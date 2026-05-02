<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Limitações conhecidas do BBS UI

- **Mobile/Termux**: alguns teclados não enviam sequências ANSI `\x1b[A` e `\x1b[B`; usar fallback `w/s`.
- **Windows Console legado**: pode não interpretar `\x1b[7m` (highlight inverso), então a renderização ASCII é preferível.
- **Pagers/remotos**: sessões com `TERM=dumb` entram em fallback ASCII automaticamente.
- **Esc isolado**: em alguns multiplexadores o `Esc` pode competir com atalhos do terminal.
