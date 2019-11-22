#[test]
fn assert_same_as_src() {
    let actual = include_str!("../../../libc-bottom-half/headers/public/wasi/api.h");
    let expected = wasi_headers::generate().expect("header generation should succeed");
    if actual == expected {
        return;
    }

    eprintln!("The following diff was found between the generated <wasi/api.h> and the");
    eprintln!("source <wasi/api.h> in the tree:");
    eprintln!();

    let mut expected_line = 1;
    let mut actual_line = 1;
    let mut separated = false;
    for diff in diff::lines(&expected, &actual) {
        match diff {
            diff::Result::Left(l) => {
                eprintln!("line {}: -{}", expected_line, l);
                expected_line += 1;
                separated = false;
            }
            diff::Result::Both(_, _) => {
                expected_line += 1;
                actual_line += 1;
                if !separated {
                    eprintln!("...");
                    separated = true;
                }
            }
            diff::Result::Right(r) => {
                eprintln!("line {}: +{}", actual_line, r);
                actual_line += 1;
                separated = false;
            }
        }
    }

    eprintln!();
    eprintln!("To regenerate the files, run `cd tools/wasi-headers && cargo run`.");
    eprintln!();
    panic!();
}
