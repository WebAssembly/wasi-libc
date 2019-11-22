use anyhow::Result;
use std::fs::File;
use std::io::Write;
use wasi_headers::generate;

pub fn main() -> Result<()> {
    let c_header = generate()?;
    let mut file = File::create("../../libc-bottom-half/headers/public/wasi/api.h")?;
    file.write_all(c_header.as_bytes())?;
    Ok(())
}
