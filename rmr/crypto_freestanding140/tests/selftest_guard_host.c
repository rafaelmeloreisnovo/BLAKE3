#include "rmr_cf140_guard.h"

#define CHECK(expr, code) do { if (!(expr)) { return (code); } } while (0)

static int test_happy_ack(void) {
    rmr_cf_guard guard;
    rmr_cf_guard_init(&guard, 2u);
    CHECK(guard.state == RMR_CF_GUARD_IDLE, 11);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_ARM) == RMR_CF_GUARD_ACTION_EXECUTE, 12);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_EXEC_OK) == RMR_CF_GUARD_ACTION_REQUEST_ACK, 13);
    CHECK(guard.state == RMR_CF_GUARD_ACK_WAIT, 14);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_ACK_OK) == RMR_CF_GUARD_ACTION_COMMIT, 15);
    CHECK(guard.state == RMR_CF_GUARD_IDLE, 16);
    return 0;
}

static int test_retry_rollback_failsafe(void) {
    rmr_cf_guard guard;
    rmr_cf_guard_init(&guard, 1u);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_ARM) == RMR_CF_GUARD_ACTION_EXECUTE, 21);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_EXEC_FAIL) == RMR_CF_GUARD_ACTION_RETRY, 22);
    CHECK(guard.retries == 1u, 23);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_WATCHDOG_EXPIRED) == RMR_CF_GUARD_ACTION_ROLLBACK, 24);
    CHECK(guard.state == RMR_CF_GUARD_ROLLBACK, 25);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_ROLLBACK_OK) == RMR_CF_GUARD_ACTION_ENTER_FAILSAFE, 26);
    CHECK(guard.state == RMR_CF_GUARD_FAILSAFE, 27);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_OPERATOR_RESET) == RMR_CF_GUARD_ACTION_RESET_ACCEPTED, 28);
    CHECK(guard.state == RMR_CF_GUARD_IDLE, 29);
    return 0;
}

static int test_fail_closed(void) {
    rmr_cf_guard guard;
    rmr_cf_guard_init(&guard, 0u);
    CHECK(rmr_cf_guard_step(&guard, 0xFFFFFFFFu) == RMR_CF_GUARD_ACTION_ENTER_FAILSAFE, 31);
    CHECK(guard.state == RMR_CF_GUARD_FAILSAFE, 32);
    rmr_cf_guard_init(&guard, 0u);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_ARM) == RMR_CF_GUARD_ACTION_EXECUTE, 33);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_WATCHDOG_EXPIRED) == RMR_CF_GUARD_ACTION_ROLLBACK, 34);
    CHECK(rmr_cf_guard_step(&guard, RMR_CF_GUARD_EVENT_ROLLBACK_FAIL) == RMR_CF_GUARD_ACTION_HALT, 35);
    CHECK(guard.state == RMR_CF_GUARD_HALTED, 36);
    CHECK(rmr_cf_guard_step((rmr_cf_guard *)0, RMR_CF_GUARD_EVENT_ARM) == RMR_CF_GUARD_ACTION_HALT, 37);
    return 0;
}

int main(void) {
    int rc = test_happy_ack();
    if (rc != 0) {
        return rc;
    }
    rc = test_retry_rollback_failsafe();
    if (rc != 0) {
        return rc;
    }
    return test_fail_closed();
}
