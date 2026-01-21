/*
Copyright (c) 2025 Rafael
License: RMR Module License (see rmr/LICENSE_RMR)
*/

#ifndef RMR_GOVERNANCE_H
#define RMR_GOVERNANCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RMR_GOVERNANCE_VERSION_MAJOR 0
#define RMR_GOVERNANCE_VERSION_MINOR 1
#define RMR_GOVERNANCE_VERSION_PATCH 0

#define RMR_GOV_META_AGENT_ID_KEY "agent_id"
#define RMR_GOV_META_TEMP_C_KEY "temp_c"
#define RMR_GOV_META_THROTTLE_KEY "throttle_ratio"
#define RMR_GOV_META_CHECKPOINT_KEY "checkpoint"
#define RMR_GOV_META_AUDIT_KEY "audit"

typedef enum rmr_governance_status {
  RMR_GOV_STATUS_OK = 0,
  RMR_GOV_STATUS_THROTTLE = 1,
  RMR_GOV_STATUS_CHECKPOINT = 2,
  RMR_GOV_STATUS_ERROR = 3
} rmr_governance_status;

typedef struct rmr_governance_thermal_policy {
  bool enabled;
  float max_celsius;
  float throttle_min_ratio;
  float throttle_max_ratio;
  uint64_t cooldown_ms;
} rmr_governance_thermal_policy;

typedef struct rmr_governance_checkpoint_policy {
  bool enabled;
  uint64_t interval_cycles;
  uint64_t min_state_bytes;
  const char *path_hint;
} rmr_governance_checkpoint_policy;

typedef struct rmr_governance_audit_policy {
  bool enabled;
  const char *path_hint;
} rmr_governance_audit_policy;

typedef struct rmr_governance_identity_policy {
  bool enabled;
  const char *static_identity;
} rmr_governance_identity_policy;

typedef float (*rmr_governance_read_temp_fn)(void *user_ctx);
typedef size_t (*rmr_governance_write_audit_fn)(void *user_ctx,
                                                const char *line,
                                                size_t line_len);
typedef bool (*rmr_governance_checkpoint_fn)(void *user_ctx,
                                             const void *state,
                                             size_t state_len);
typedef const char *(*rmr_governance_identity_fn)(void *user_ctx);

typedef struct rmr_governance_callbacks {
  rmr_governance_read_temp_fn read_temp_c;
  rmr_governance_write_audit_fn write_audit_line;
  rmr_governance_checkpoint_fn checkpoint_state;
  rmr_governance_identity_fn identity;
} rmr_governance_callbacks;

typedef struct rmr_governance_config {
  rmr_governance_thermal_policy thermal;
  rmr_governance_checkpoint_policy checkpoint;
  rmr_governance_audit_policy audit;
  rmr_governance_identity_policy identity;
  rmr_governance_callbacks callbacks;
  void *user_ctx;
} rmr_governance_config;

typedef struct rmr_governance_stats {
  uint64_t cycles;
  uint64_t bytes_processed;
  uint64_t last_checkpoint_cycle;
  float last_temp_c;
  float last_throttle_ratio;
} rmr_governance_stats;

typedef struct rmr_governance_ctx {
  rmr_governance_config config;
  rmr_governance_stats stats;
} rmr_governance_ctx;

bool rmr_governance_init(rmr_governance_ctx *ctx,
                         const rmr_governance_config *config);

rmr_governance_status rmr_governance_on_chunk(rmr_governance_ctx *ctx,
                                              size_t bytes_processed,
                                              uint64_t now_ms);

rmr_governance_status rmr_governance_checkpoint(rmr_governance_ctx *ctx,
                                                const void *state,
                                                size_t state_len);

size_t rmr_governance_emit_metadata(const rmr_governance_ctx *ctx,
                                    char *out,
                                    size_t out_len);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
