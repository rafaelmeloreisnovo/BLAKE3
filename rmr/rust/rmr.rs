//! RMR module (experimental).
//!
//! * Copyright (c) 2024–2026 Rafael Melo Reis
//! * Licensed under LICENSE_RMR.
//! *
//! * This file is part of the RMR module.
//! It does not modify or replace the BLAKE3 core.
 


//! Este módulo define uma base mínima para futuras otimizações e
//! extensões de arquitetura relacionadas a desempenho. Ele é um ponto
//! de integração isolado para evitar mudanças amplas no restante do
//! crate.

/// Identificador de versão do módulo RMR.
pub const RMR_VERSION: &str = "0.2.0";

/// Tamanho do buffer de leitura para IO amplo.
pub(crate) const IO_READ_BUF_LEN: usize = 128 * 1024;

/// Estrutura de configuração para evoluções futuras do módulo RMR.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct RmrConfig {
    /// Habilita ajustes experimentais de performance.
    pub experimental: bool,
}

impl Default for RmrConfig {
    #[inline]
    fn default() -> Self {
        Self {
            experimental: false,
        }
    }
}
