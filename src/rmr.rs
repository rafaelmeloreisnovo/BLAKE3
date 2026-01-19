//! RMR module (experimental).
//!
//! Este módulo define uma base mínima para futuras otimizações e
//! extensões de arquitetura relacionadas a desempenho. Ele é um ponto
//! de integração isolado para evitar mudanças amplas no restante do
//! crate.

/// Identificador de versão do módulo RMR.
pub const RMR_VERSION: &str = "0.1.0";

/// Estrutura de configuração para evoluções futuras do módulo RMR.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct RmrConfig {
    /// Habilita ajustes experimentais de performance.
    pub experimental: bool,
}

impl Default for RmrConfig {
    fn default() -> Self {
        Self {
            experimental: false,
        }
    }
}
