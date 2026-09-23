// Copyright (c) 2024-2026 Rafael Melo Reis
// Licensed under LICENSE_RMR.

use std::env;
use std::time::Instant;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("usage: rmr_validation_bench SIZE ITERATIONS");
        std::process::exit(2);
    }
    let size: usize = args[1].parse().unwrap();
    let iterations: u64 = args[2].parse().unwrap();
    assert!(size > 0 && iterations > 0);

    let mut input = vec![0u8; size];
    let mut state: u32 = 0x9e3779b9;
    for b in &mut input {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        *b = state as u8;
    }

    let mut guard: u8 = 0;
    for _ in 0..4 {
        guard ^= blake3::hash(&input).as_bytes()[0];
    }

    let start = Instant::now();
    let mut last = blake3::Hash::from([0u8; 32]);
    for i in 0..iterations {
        last = blake3::hash(&input);
        guard ^= last.as_bytes()[(i as usize) & 31];
    }
    let elapsed = start.elapsed();
    let ns = elapsed.as_secs_f64() * 1e9;
    let ns_per_op = ns / iterations as f64;
    let mib_s = (size as f64 * iterations as f64 / (1024.0 * 1024.0))
        / elapsed.as_secs_f64();

    println!(
        "RUST_RESULT,{},{},{:.3},{:.3},{:02x},{}",
        size,
        iterations,
        ns_per_op,
        mib_s,
        guard,
        last.to_hex()
    );
}
