// Measurement-only harness. Neither BLAKE3 cryptographic core is modified.
// This is hosted Rust timing, not bare-metal/device execution.
use std::env;
use std::hint::black_box;
use std::time::Instant;

fn main() {
    let mut args = env::args().skip(1);
    let size: usize = args.next().expect("size").parse().expect("size_integer");
    let iterations: usize = args.next().expect("iterations").parse().expect("iterations_integer");
    assert!(size > 0 && size <= 1_048_576 && iterations > 0);
    assert!(args.next().is_none());
    let mut input = vec![0_u8; size];
    let mut state: u32 = 0x9e37_79b9;
    for byte in &mut input {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        *byte = state as u8;
    }
    let expected = blake3::hash(&input).to_hex().to_string();
    for _ in 0..4 {
        black_box(blake3::hash(black_box(&input)));
    }
    let mut guard: u32 = 0;
    let start = Instant::now();
    for i in 0..iterations {
        let digest = blake3::hash(black_box(&input));
        guard ^= digest.as_bytes()[i % 32] as u32;
    }
    let elapsed = start.elapsed().as_nanos();
    black_box(guard);
    assert!(elapsed > 0);
    println!("{elapsed},{expected},{guard}");
}
