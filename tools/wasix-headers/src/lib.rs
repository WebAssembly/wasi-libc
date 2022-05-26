mod c_header;

use anyhow::Result;
pub use c_header::{to_c, Generated};
use std::path::{Path, PathBuf};
use witx::load;

pub fn generate<P: AsRef<Path>>(inputs: &[P], is64bit: bool) -> Result<Generated> {
    let doc = load(&inputs, is64bit)?;

    let inputs_str = &inputs
        .iter()
        .map(|p| {
            p.as_ref()
                .file_name()
                .unwrap()
                .to_str()
                .unwrap()
                .to_string()
        })
        .collect::<Vec<_>>()
        .join(", ");

    Ok(to_c(&doc, &inputs_str, is64bit))
}

pub fn snapshot_witx_files() -> Result<Vec<PathBuf>> {
    witx::phases::snapshot()
}

pub fn libc_wasi_api_header() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("../../libc-bottom-half/headers/public/wasi/api.h")
}

pub fn libc_wasi_api_source() -> PathBuf {
    PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("../../libc-bottom-half/sources/__wasilibc_real.c")
}
