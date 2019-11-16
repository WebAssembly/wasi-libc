mod c_header;

use crate::c_header::to_c_header;
use clap::{App, Arg};
use std::fs::File;
use std::io::Write;
use std::path::PathBuf;
use std::process;
use witx::load;

pub fn main() {
    let app = App::new("wasi-headers")
        .version(env!("CARGO_PKG_VERSION"))
        .about("Generate C headers for WASI interfaces")
        .arg(
            Arg::with_name("input")
                .required(true)
                .multiple(true)
                .help("path to root of witx document"),
        )
        .arg(
            Arg::with_name("verbose")
                .short("v")
                .long("verbose")
                .takes_value(false)
                .required(false),
        )
        .get_matches();

    let inputs = app
        .values_of("input")
        .expect("at least one input required")
        .map(PathBuf::from)
        .collect::<Vec<PathBuf>>();

    match load(&inputs) {
        Ok(doc) => {
            if app.is_present("verbose") {
                println!("{:?}", doc)
            }

            let c_header = to_c_header(&doc);
            if let Some(output) = app.value_of("output") {
                let mut file = File::create(output).expect("create output file");
                file.write_all(c_header.as_bytes())
                    .expect("write output file");
            } else {
                println!("{}", c_header)
            }
        }
        Err(e) => {
            println!("{}", e.report());
            if app.is_present("verbose") {
                println!("{:?}", e);
            }
            process::exit(1)
        }
    }
}
