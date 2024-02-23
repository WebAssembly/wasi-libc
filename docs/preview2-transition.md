# WASI Preview Transition

How will wasi-libc transition to WASI preview 2? This document outlines a plan
and describes how to maintain preview 1 support at the same time. This document
will _not_ discuss how and if and when preview 1 support could be dropped from
the repository; the WASI subgroup is a better forum for that.

There seem to be several parts to this:

1. __new targets__: to avoid confusion, let's adopt the target names agreed to
   elsewhere: `wasm32-wasip1`, `wasm32-wasip2`. This means that new releases of
   wasi-libc/wasi-sdk would contain new `wasm32-wasip*` subdirectories and users
   could compile with `--target wasm32-wasip*`. The existing `wasm32-wasi` and
   `wasm32-wasi-threads` targets can continue to coexist until some undefined
   future date (note that the contents of `wasm32-wasip1` and `wasm32-wasi`
   should be identical). Future releases of WASI, e.g., 0.2.1, 0.2.2, 0.2.x,
   will result in new `wasm32-wasip2.*` subdirectories.

2. __new internal macro definitions__: internally in wasi-libc, let's use
   `#ifdef __wasilibc_wasip*` blocks to conditionally compile code that depends
   on a specific version of WASI. The `wasip*` bit should match the target
   suffix. The preference should be to keep wasi-libc free from complicated
   version logic &mdash; ideally, most wasi-libc code should be generic across
   versions. The `__wasilibc_wasip*` macro definitions exist to clearly identify
   which parts belong to each preview (e.g., this should make maintenance easier
   for those interested in supporting preview 1). Since we expect ABI
   compatibility between previews, all prior versions should be defined; e.g.,
   for target `wasip2.1`, both `__wasilibc_wasip2` and `__waslibc_wasip2.1`
   would be defined but `__wasilibc_wasip1` would not.  PR [#449] proposed
   `__wasilibc_use_preview2` but (a) `...wasip*` lines up the names with the
   targets, making "what gets compiled where" more clear and (b) we will likely
   need the ability to specify which precise version a block applies to in a
   fine-grained way (e.g., `#ifdef __wasilibc_wasip0.2.2`).

[#449]: https://github.com/WebAssembly/wasi-libc/pull/449

3. __new user-facing macro definitions__: externally, users of wasi-libc may
   want to conditionally compile their code based on the WASI version: let's
   define `__WASI_VERSION` as the preview version string, e.g., `"p1"` for
   preview 1, `"p2"` for initial preview 2 support, `"p2.x"` for future WASI
   releases. To avoid including this definition in multiple headers, we could
   (a) initially include it in `libc-bottom-half/headers/public/wasi/api.h`,
   requiring users to `#include <wasi/api.h>` and (b) discuss whether to
   upstream general support for this in Clang.

4. __modified Clang ABI__: since the transition to preview 2 involves an ABI
   change (i.e., to the canonical ABI), this seems like a good point to
   introduce other Clang-level ABI changes. Let's allow the Clang ABI to use
   multi-value returns and discuss any other ABI changes during this time.

Note that the preview 1 parts of this plan are hopefully temporary: Marcin Kolny
has [plans] to attempt a preview2-to-preview1 adapter which might be able to
avoid extra `#ifdef`s in this project.

[plans]: https://github.com/loganek/wasi-snapshot-preview2-to-preview1-adapter
