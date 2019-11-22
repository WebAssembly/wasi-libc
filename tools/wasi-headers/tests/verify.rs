#[test]
fn assert_same_as_src() {
    let actual = include_str!("../../../libc-bottom-half/headers/public/wasi/api.h");
    let expected = wasi_headers::generate().expect("header generation should succeed");
    if actual == expected {
        return;
    }
    panic!(
        "

the generate `<wasi/api.h>` does not match the actual source `<wasi/api.h>`, it's
recommended to run this command from the root of the repository:

    cd tools/wasi-headers && cargo run

"
    );
}
