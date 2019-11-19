mod c_header;

use crate::c_header::to_c_header;
use anyhow::{anyhow, Result};
use std::fs::{read_dir, File};
use std::io::{self, Write};
use witx::load;

pub fn main() -> Result<()> {
    let inputs = read_dir("WASI/phases/snapshot/witx")?
        .map(|res| res.map(|e| e.path()))
        .collect::<Result<Vec<_>, io::Error>>()?;

    // TODO: drop the anyhow! part once witx switches to anyhow.
    let doc = load(&inputs).map_err(|e| anyhow!(e.to_string()))?;

    let inputs_str = &inputs
        .iter()
        .map(|p| p.file_name().unwrap().to_str().unwrap().to_string())
        .collect::<Vec<_>>()
        .join(", ");

    let c_header = to_c_header(&doc, &inputs_str);
    let mut file = File::create("../../libc-bottom-half/headers/public/wasi/api.h")
        .expect("create output file");
    file.write_all(c_header.as_bytes())
        .expect("write output file");
    Ok(())
}
