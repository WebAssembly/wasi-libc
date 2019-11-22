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
    let mut any_lines = false;
    for diff in diff::lines(&expected, &actual) {
        match diff {
            diff::Result::Left(l) => {
                eprintln!("line {}: -{}", expected_line, l);
                expected_line += 1;
                separated = false;
                any_lines = true;
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
                any_lines = true;
            }
        }
    }

    if !any_lines {
        eprintln!();
        eprintln!(
            "Somehow there was a diff with no lines differing. Lengths: {} and {}.",
            expected.len(),
            actual.len()
        );
        eprintln!();
        eprintln!("actual: {}", actual);
        eprintln!();
        eprintln!("expected: {}", expected);
    }

    eprintln!();
    eprintln!("To regenerate the files, run `cd tools/wasi-headers && cargo run`.");
    eprintln!();
    panic!();
}
