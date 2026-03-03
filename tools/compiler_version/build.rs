/*
 * Copyright (c) 2025 Rafael
 * License: RMR Module License (see rmr/LICENSE_RMR)
 */

fn main() {
    let build = cc::Build::new();
    let compiler = build.get_compiler();
    let compiler_path = compiler.path().to_string_lossy();
    println!("cargo:rustc-env=COMPILER_PATH={}", compiler_path);
}
