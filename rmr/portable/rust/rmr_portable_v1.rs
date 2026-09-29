// Copyright (c) 2026 Rafael Melo Reis.
// SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
#![no_std]

pub const RMR_PV1_FRAME_BYTES: usize = 256;
pub const RMR_PV1_REDUCED_BYTES: usize = 16;

unsafe extern "C" {
    fn rmr_pv1_reduce256(out: *mut u8, input: *const u8);
    fn rmr_pv1_equal16(a: *const u8, b: *const u8) -> u32;
}

#[inline(always)]
pub unsafe fn reduce256(out: *mut u8, input: *const u8) {
    unsafe { rmr_pv1_reduce256(out, input) }
}

#[inline(always)]
pub unsafe fn equal16(a: *const u8, b: *const u8) -> u32 {
    unsafe { rmr_pv1_equal16(a, b) }
}
