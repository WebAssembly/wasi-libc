#[macro_use]
extern crate clap;

use anyhow::Result;
use clap::{Arg, SubCommand};
use std::fs::File;
use std::io::Write;
use std::path::PathBuf;
use wasi_headers::{generate, libc_wasi_api_header, snapshot_witx_files};

struct GenerateCommand {
    /// Input witx file
    inputs: Vec<PathBuf>,
    /// Output header file
    output: PathBuf,
}

impl GenerateCommand {
    pub fn execute(&self) -> Result<()> {
        let c_header = generate(&self.inputs)?;
        let mut file = File::create(&self.output)?;
        file.write_all(c_header.as_bytes())?;
        Ok(())
    }
}

fn main() -> Result<()> {
    let matches = app_from_crate!()
        .arg(Arg::with_name("inputs").required(false).multiple(true))
        .arg(
            Arg::with_name("output")
                .short("o")
                .long("output")
                .takes_value(true)
                .required(false),
        )
        .subcommand(
            SubCommand::with_name("generate-libc")
                .about("generate libc api.h from current snapshot"),
        )
        .get_matches();

    let cmd = if matches.subcommand_matches("generate-libc").is_some() {
        let inputs = snapshot_witx_files()?;
        let output = libc_wasi_api_header();
        GenerateCommand { inputs, output }
    } else {
        GenerateCommand {
            inputs: matches
                .values_of("inputs")
                .expect("inputs required")
                .map(PathBuf::from)
                .collect(),
            output: PathBuf::from(matches.value_of("output").expect("output required")),
        }
    };

    cmd.execute()?;
    Ok(())
}
