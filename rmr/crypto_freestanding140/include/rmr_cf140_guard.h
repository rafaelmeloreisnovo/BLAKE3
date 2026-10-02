/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * RMR Crypto Freestanding 140 — critical supervisor contract V1.
 * No libc. No heap. No OS. No clock. Events are caller supplied.
 */
#ifndef RMR_CF140_GUARD_H
#define RMR_CF140_GUARD_H

#include "rmr_cf140.h"

#define RMR_CF_GUARD_VERSION 1u
#define RMR_CF_GUARD_RETRY_LIMIT 7u

#define RMR_CF_GUARD_IDLE 0u
#define RMR_CF_GUARD_RUNNING 1u
#define RMR_CF_GUARD_ACK_WAIT 2u
#define RMR_CF_GUARD_ROLLBACK 3u
#define RMR_CF_GUARD_FAILSAFE 4u
#define RMR_CF_GUARD_HALTED 5u

#define RMR_CF_GUARD_EVENT_ARM 1u
#define RMR_CF_GUARD_EVENT_EXEC_OK 2u
#define RMR_CF_GUARD_EVENT_EXEC_FAIL 3u
#define RMR_CF_GUARD_EVENT_ACK_OK 4u
#define RMR_CF_GUARD_EVENT_ACK_FAIL 5u
#define RMR_CF_GUARD_EVENT_WATCHDOG_EXPIRED 6u
#define RMR_CF_GUARD_EVENT_ROLLBACK_OK 7u
#define RMR_CF_GUARD_EVENT_ROLLBACK_FAIL 8u
#define RMR_CF_GUARD_EVENT_OPERATOR_RESET 9u

#define RMR_CF_GUARD_ACTION_NONE 0u
#define RMR_CF_GUARD_ACTION_EXECUTE 1u
#define RMR_CF_GUARD_ACTION_REQUEST_ACK 2u
#define RMR_CF_GUARD_ACTION_COMMIT 3u
#define RMR_CF_GUARD_ACTION_RETRY 4u
#define RMR_CF_GUARD_ACTION_ROLLBACK 5u
#define RMR_CF_GUARD_ACTION_ENTER_FAILSAFE 6u
#define RMR_CF_GUARD_ACTION_HALT 7u
#define RMR_CF_GUARD_ACTION_RESET_ACCEPTED 8u

typedef struct rmr_cf_guard {
    rmr_cf_u32 state;
    rmr_cf_u32 retries;
    rmr_cf_u32 max_retries;
    rmr_cf_u32 sequence;
    rmr_cf_u32 last_event;
} rmr_cf_guard;

void rmr_cf_guard_init(rmr_cf_guard *guard, rmr_cf_u32 max_retries);
rmr_cf_u32 rmr_cf_guard_step(rmr_cf_guard *guard, rmr_cf_u32 event);

#endif
