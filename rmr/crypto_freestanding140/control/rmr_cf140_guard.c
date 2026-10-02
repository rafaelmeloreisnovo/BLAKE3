#include "rmr_cf140_guard.h"

static rmr_cf_u32 rmr_cf_guard_retry_or_rollback(rmr_cf_guard *guard) {
    if (guard->retries < guard->max_retries) {
        guard->retries += 1u;
        guard->state = RMR_CF_GUARD_RUNNING;
        return RMR_CF_GUARD_ACTION_RETRY;
    }
    guard->state = RMR_CF_GUARD_ROLLBACK;
    return RMR_CF_GUARD_ACTION_ROLLBACK;
}

static rmr_cf_u32 rmr_cf_guard_fail_closed(rmr_cf_guard *guard) {
    guard->state = RMR_CF_GUARD_FAILSAFE;
    return RMR_CF_GUARD_ACTION_ENTER_FAILSAFE;
}

void rmr_cf_guard_init(rmr_cf_guard *guard, rmr_cf_u32 max_retries) {
    if (guard == (rmr_cf_guard *)0) {
        return;
    }
    guard->state = RMR_CF_GUARD_IDLE;
    guard->retries = 0u;
    guard->max_retries = max_retries <= RMR_CF_GUARD_RETRY_LIMIT
        ? max_retries
        : RMR_CF_GUARD_RETRY_LIMIT;
    guard->sequence = 0u;
    guard->last_event = 0u;
}

rmr_cf_u32 rmr_cf_guard_step(rmr_cf_guard *guard, rmr_cf_u32 event) {
    if (guard == (rmr_cf_guard *)0) {
        return RMR_CF_GUARD_ACTION_HALT;
    }

    guard->sequence += 1u;
    guard->last_event = event;

    switch (guard->state) {
        case RMR_CF_GUARD_IDLE:
            if (event == RMR_CF_GUARD_EVENT_ARM) {
                guard->state = RMR_CF_GUARD_RUNNING;
                guard->retries = 0u;
                return RMR_CF_GUARD_ACTION_EXECUTE;
            }
            return rmr_cf_guard_fail_closed(guard);

        case RMR_CF_GUARD_RUNNING:
            if (event == RMR_CF_GUARD_EVENT_EXEC_OK) {
                guard->state = RMR_CF_GUARD_ACK_WAIT;
                return RMR_CF_GUARD_ACTION_REQUEST_ACK;
            }
            if ((event == RMR_CF_GUARD_EVENT_EXEC_FAIL) ||
                (event == RMR_CF_GUARD_EVENT_WATCHDOG_EXPIRED)) {
                return rmr_cf_guard_retry_or_rollback(guard);
            }
            return rmr_cf_guard_fail_closed(guard);

        case RMR_CF_GUARD_ACK_WAIT:
            if (event == RMR_CF_GUARD_EVENT_ACK_OK) {
                guard->state = RMR_CF_GUARD_IDLE;
                guard->retries = 0u;
                return RMR_CF_GUARD_ACTION_COMMIT;
            }
            if ((event == RMR_CF_GUARD_EVENT_ACK_FAIL) ||
                (event == RMR_CF_GUARD_EVENT_WATCHDOG_EXPIRED)) {
                return rmr_cf_guard_retry_or_rollback(guard);
            }
            return rmr_cf_guard_fail_closed(guard);

        case RMR_CF_GUARD_ROLLBACK:
            if (event == RMR_CF_GUARD_EVENT_ROLLBACK_OK) {
                guard->state = RMR_CF_GUARD_FAILSAFE;
                return RMR_CF_GUARD_ACTION_ENTER_FAILSAFE;
            }
            if (event == RMR_CF_GUARD_EVENT_ROLLBACK_FAIL) {
                guard->state = RMR_CF_GUARD_HALTED;
                return RMR_CF_GUARD_ACTION_HALT;
            }
            return rmr_cf_guard_fail_closed(guard);

        case RMR_CF_GUARD_FAILSAFE:
            if (event == RMR_CF_GUARD_EVENT_OPERATOR_RESET) {
                guard->state = RMR_CF_GUARD_IDLE;
                guard->retries = 0u;
                return RMR_CF_GUARD_ACTION_RESET_ACCEPTED;
            }
            return RMR_CF_GUARD_ACTION_ENTER_FAILSAFE;

        case RMR_CF_GUARD_HALTED:
            return RMR_CF_GUARD_ACTION_HALT;

        default:
            return rmr_cf_guard_fail_closed(guard);
    }
}
