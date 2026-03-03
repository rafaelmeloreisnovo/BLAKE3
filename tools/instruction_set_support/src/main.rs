/*
 * Copyright (c) 2025 Rafael
 * License: RMR Module License (see rmr/LICENSE_RMR)
 */

fn main() {
    #[cfg(any(target_arch = "x86", target_arch = "x86_64"))]
    {
        dbg!(is_x86_feature_detected!("sse2"));
        dbg!(is_x86_feature_detected!("sse4.1"));
        dbg!(is_x86_feature_detected!("avx2"));
        dbg!(is_x86_feature_detected!("avx512f"));
        dbg!(is_x86_feature_detected!("avx512vl"));
    }
}
