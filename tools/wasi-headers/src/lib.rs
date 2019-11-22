mod c_header;

use anyhow::{anyhow, Result};
use c_header::to_c_header;
use std::fs::read_dir;
use std::io;
use witx::load;

pub fn generate() -> Result<String> {
    let mut inputs = read_dir("WASI/phases/snapshot/witx")?
        .map(|res| res.map(|e| e.path()))
        .collect::<Result<Vec<_>, io::Error>>()?;

    inputs.sort();

    // TODO: drop the anyhow! part once witx switches to anyhow.
    let doc = load(&inputs).map_err(|e| anyhow!(e.to_string()))?;

    let inputs_str = &inputs
        .iter()
        .map(|p| p.file_name().unwrap().to_str().unwrap().to_string())
        .collect::<Vec<_>>()
        .join(", ");

    Ok(to_c_header(&doc, &inputs_str))
}
