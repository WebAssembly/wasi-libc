#[macro_use]
extern crate clap;

use anyhow::Result;
use clap::{Arg, SubCommand};
use std::fs;
use std::path::Path;
use wasi_headers::{generate, libc_wasi_api_header, libc_wasi_api_source, snapshot_witx_files};

pub fn run(
    inputs: &[impl AsRef<Path>],
    header_output: impl AsRef<Path>,
    source_output: impl AsRef<Path>,
) -> Result<()> {
    let c_header = generate(inputs)?;
    fs::write(header_output, c_header.header)?;
    fs::write(source_output, c_header.source)?;
    Ok(())
}

fn main() -> Result<()> {
    let matches = app_from_crate!()
        .setting(clap::AppSettings::SubcommandRequiredElseHelp)
        .subcommand(
            SubCommand::with_name("generate")
                .arg(Arg::with_name("inputs").required(true).multiple(true))
                .arg(
                    Arg::with_name("output")
                        .short("o")
                        .long("output")
                        .takes_value(true)
                        .required(true),
                ),
        )
        .subcommand(
            SubCommand::with_name("generate-libc")
                .about("generate libc api.h from current snapshot"),
        )
        .get_matches();

    if matches.subcommand_matches("generate-libc").is_some() {
        let inputs = snapshot_witx_files()?;
        run(&inputs, libc_wasi_api_header(), libc_wasi_api_source())?;
    } else if let Some(generate) = matches.subcommand_matches("generate") {
        let inputs = generate
            .values_of("inputs")
            .expect("required inputs arg")
            .collect::<Vec<_>>();
        let output = generate.value_of("output").expect("required output arg");
        let output = Path::new(&output);
        run(
            &inputs,
            output.with_extension("h"),
            output.with_extension("c"),
        )?;
    } else {
        unreachable!("a subcommand must be provided")
    };

    Ok(())
}
