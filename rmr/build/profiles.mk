# Centralized build profiles for RMR external layer.
# shellcheck shell=sh

: "${RMR_BUILD_PROFILE:=throughput}"

# Base policy knobs (shared).
RMR_BASE_CFLAGS="-ffreestanding -fno-stack-protector"
RMR_BASE_LDFLAGS="-nostdlib -Wl,-e,_start -pie"

# Profile map.
RMR_PROFILE_latency_CFLAGS="-O2"
RMR_PROFILE_throughput_CFLAGS="-O3"
RMR_PROFILE_deterministic_CFLAGS="-O2 -fno-builtin"
RMR_PROFILE_debug_CFLAGS="-O0 -g3"

# Link policy defaults.
RMR_PROFILE_latency_LDFLAGS="$RMR_BASE_LDFLAGS"
RMR_PROFILE_throughput_LDFLAGS="$RMR_BASE_LDFLAGS"
RMR_PROFILE_deterministic_LDFLAGS="$RMR_BASE_LDFLAGS"
RMR_PROFILE_debug_LDFLAGS="$RMR_BASE_LDFLAGS"

rmr_select_profile() {
  profile="${1:-$RMR_BUILD_PROFILE}"
  case "$profile" in
    latency|throughput|deterministic|debug) ;;
    *) echo "invalid RMR_BUILD_PROFILE: $profile" >&2; return 1 ;;
  esac

  eval "RMR_OPT_CFLAGS=\${RMR_PROFILE_${profile}_CFLAGS}"
  eval "RMR_OPT_LDFLAGS=\${RMR_PROFILE_${profile}_LDFLAGS}"

  RMR_FINAL_CFLAGS="${RMR_BASE_CFLAGS} ${RMR_OPT_CFLAGS} ${RMR_EXTRA_CFLAGS:-}"
  RMR_FINAL_LDFLAGS="${RMR_OPT_LDFLAGS} ${RMR_EXTRA_LDFLAGS:-}"
  export RMR_FINAL_CFLAGS RMR_FINAL_LDFLAGS RMR_BUILD_PROFILE

  # Explicit policy:
  # -O2: latency/deterministic/debug baseline and predictable codegen.
  # -O3: throughput profile only.
  # -fno-builtin: required for deterministic profile; optional elsewhere.
  # -fPIC: allowed only for host/shared builds via RMR_EXTRA_CFLAGS.
  # -pie: required by default for freestanding executables.
  # -nostdlib: required by default for freestanding executables.
}
