use heck::ShoutySnakeCase;
use std::collections::HashMap;
use witx::*;

pub struct Generated {
    pub header: String,
    pub source: String,
}

pub fn to_c(doc: &Document, inputs_str: &str) -> Generated {
    let mut header = String::new();

    header.push_str(&format!(
        r#"/**
 * <wasi/api.h>. This file contains declarations describing the WASI ABI
 * as of "snapshot preview1". It was originally auto-generated from
 * {}, however WASI is in the process of
 * transitioning to a new IDL and header file generator, and this file
 * is temporarily being manually maintained.
 *
 * @file
 * This file describes the [WASI] interface, consisting of functions, types,
 * and defined values (macros).
 *
 * The interface described here is greatly inspired by [CloudABI]'s clean,
 * thoughtfully-designed, capability-oriented, POSIX-style API.
 *
 * [CloudABI]: https://github.com/NuxiNL/cloudlibc
 * [WASI]: https://github.com/WebAssembly/WASI/
 */

#ifndef __wasi_api_h
#define __wasi_api_h

#ifndef __wasi__
#error <wasi/api.h> is only supported on WASI platforms.
#endif

#include <stddef.h>
#include <stdint.h>

_Static_assert(_Alignof(int8_t) == 1, "non-wasi data layout");
_Static_assert(_Alignof(uint8_t) == 1, "non-wasi data layout");
_Static_assert(_Alignof(int16_t) == 2, "non-wasi data layout");
_Static_assert(_Alignof(uint16_t) == 2, "non-wasi data layout");
_Static_assert(_Alignof(int32_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(uint32_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(int64_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(uint64_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(void*) == sizeof(intptr_t), "non-wasi data layout");

#ifdef __cplusplus
extern "C" {{
#endif

#ifdef __cplusplus
#if __cplusplus >= 201103L
#define __WASI_NOEXCEPT noexcept
#else
#define __WASI_NOEXCEPT throw()
#endif
#else
#define __WASI_NOEXCEPT
#endif

// TODO: Encoding this in witx.
#define __WASI_DIRCOOKIE_START (UINT64_C(0))
"#,
        inputs_str,
    ));

    let mut source = String::new();
    source.push_str(&format!(
        r#"/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   {}
 *
 * To regenerate this file execute:
 *
 *     cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
 *
 * Modifications to this file will cause CI to fail, the code generator tool
 * must be modified to change this file.
 */

#include <wasi/api.h>
#include <string.h>

#ifdef __wasm64__
#define IMPORT_NAME(x) __import_name__(x "_i64")
#else
#define IMPORT_NAME(x) __import_name__(x)
#endif

#ifdef __cplusplus
extern "C" {{
#endif

"#,
        inputs_str,
    ));

    let mut type_constants = HashMap::new();
    for c in doc.constants() {
        type_constants.entry(&c.ty).or_insert(Vec::new()).push(c);
    }

    for nt in doc.typenames() {
        print_datatype(&mut header, &*nt);

        if let Some(constants) = type_constants.remove(&nt.name) {
            for constant in constants {
                print_constant(&mut header, &constant);
            }
        }
    }

    for m in doc.modules() {
        print_module(&mut header, &mut source, &m);
    }

    header.push_str(
        r#"#ifdef _REENTRANT
/**
    * Request a new thread to be created by the host.
    *
    * The host will create a new instance of the current module sharing its
    * memory, find an exported entry function--`wasi_thread_start`--, and call the
    * entry function with `start_arg` in the new thread.
    *
    * @see https://github.com/WebAssembly/wasi-threads/#readme
    */
int32_t __wasi_thread_spawn(
    /**
        * A pointer to an opaque struct to be passed to the module's entry
        * function.
        */
    void *start_arg
) __WASI_NOEXCEPT  __attribute__((__warn_unused_result__));
#endif

#ifdef __cplusplus
}
#endif

#endif
"#,
    );

    source.push_str(
        r#"#ifdef _REENTRANT
uint32_t __imported_wasi_thread_spawn(intptr_t arg0) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi"),
    IMPORT_NAME("thread-spawn")
));

int32_t __wasi_thread_spawn(void* start_arg) __WASI_NOEXCEPT {
    return (int32_t) __imported_wasi_thread_spawn((intptr_t) start_arg);
}
#endif

#ifdef __cplusplus
}
#endif
"#,
    );

    Generated { header, source }
}

fn print_datatype(ret: &mut String, nt: &NamedType) {
    if !nt.docs.is_empty() {
        ret.push_str("/**\n");
        for line in nt.docs.lines() {
            ret.push_str(&format!(" * {}\n", line));
        }
        ret.push_str(" */\n");
    }

    match &nt.tref {
        TypeRef::Value(v) => match &**v {
            Type::Record(s) => print_record(ret, &nt.name, s),
            Type::Variant(v) => print_variant(ret, &nt.name, v),
            Type::Handle(h) => print_handle(ret, &nt.name, h),
            Type::Builtin { .. }
            | Type::List { .. }
            | Type::Pointer { .. }
            | Type::ConstPointer { .. } => print_alias(ret, &nt.name, &nt.tref),
        },
        TypeRef::Name(_) => print_alias(ret, &nt.name, &nt.tref),
    }
}

fn print_alias(ret: &mut String, name: &Id, dest: &TypeRef) {
    match &**dest.type_() {
        Type::List(_) => {
            // Don't emit arrays as top-level types; instead we special-case
            // them in places like parameter lists so that we can pass them
            // as pointer and length pairs.
        }
        _ => {
            if name.as_str() == "size" {
                // Special-case "size" as "__SIZE_TYPE__" -- TODO: Encode this in witx.
                ret.push_str(&format!(
                    "typedef __SIZE_TYPE__ __wasi_{}_t;\n",
                    ident_name(name)
                ));
            } else {
                ret.push_str(&format!(
                    "typedef {} __wasi_{}_t;\n",
                    typeref_name(dest),
                    ident_name(name)
                ));
            }
            ret.push_str("\n");
            /*
                        ret.push_str(&format!(
                            "_Static_assert(sizeof(__wasi_{}_t) == {}, \"witx calculated size\");\n",
                            ident_name(name),
                            dest.mem_size_align().size
                        ));
                        ret.push_str(&format!(
                            "_Static_assert(_Alignof(__wasi_{}_t) == {}, \"witx calculated align\");\n",
                            ident_name(name),
                            dest.mem_size_align().align
                        ));
            */
            ret.push_str("\n");
        }
    }
}

fn print_enum(ret: &mut String, name: &Id, v: &Variant) {
    ret.push_str(&format!(
        "typedef {} __wasi_{}_t;\n",
        intrepr_name(v.tag_repr),
        ident_name(name)
    ));
    ret.push_str("\n");

    for (index, case) in v.cases.iter().enumerate() {
        if !case.docs.is_empty() {
            ret.push_str("/**\n");
            for line in case.docs.lines() {
                ret.push_str(&format!(" * {}\n", line));
            }
            ret.push_str(" */\n");
        }
        ret.push_str(&format!(
            "#define __WASI_{}_{} ({}({}))\n",
            ident_name(&name).to_shouty_snake_case(),
            ident_name(&case.name).to_shouty_snake_case(),
            intrepr_const(v.tag_repr),
            index
        ));
        ret.push_str("\n");
    }
    /*
        ret.push_str(&format!(
            "_Static_assert(sizeof(__wasi_{}_t) == {}, \"witx calculated size\");\n",
            ident_name(name),
            v.tag_repr.mem_size()
        ));
        ret.push_str(&format!(
            "_Static_assert(_Alignof(__wasi_{}_t) == {}, \"witx calculated align\");\n",
            ident_name(name),
            v.tag_repr.mem_align()
        ));
    */
    ret.push_str("\n");
}

fn print_constant(ret: &mut String, const_: &Constant) {
    if !const_.docs.is_empty() {
        ret.push_str("/**\n");
        for line in const_.docs.lines() {
            ret.push_str(&format!(" * {}\n", line));
        }
        ret.push_str(" */\n");
    }
    ret.push_str(&format!(
        "#define __WASI_{}_{} ((__wasi_{}_t){})\n",
        ident_name(&const_.ty).to_shouty_snake_case(),
        ident_name(&const_.name).to_shouty_snake_case(),
        ident_name(&const_.ty),
        const_.value,
    ));
    ret.push_str("\n");
}

fn print_record(ret: &mut String, name: &Id, s: &RecordDatatype) {
    if let Some(repr) = s.bitflags_repr() {
        ret.push_str(&format!(
            "typedef {} __wasi_{}_t;\n\n",
            intrepr_name(repr),
            ident_name(name)
        ));
        for (i, member) in s.members.iter().enumerate() {
            if !member.docs.is_empty() {
                ret.push_str("/**\n");
                for line in member.docs.lines() {
                    ret.push_str(&format!(" * {}\n", line));
                }
                ret.push_str(" */\n");
            }
            ret.push_str(&format!(
                "#define __WASI_{}_{} ((__wasi_{}_t)(1 << {}))\n",
                ident_name(name).to_shouty_snake_case(),
                ident_name(&member.name).to_shouty_snake_case(),
                ident_name(name),
                i,
            ));
            ret.push_str("\n");
        }
        return;
    }
    ret.push_str(&format!(
        "typedef struct __wasi_{}_t {{\n",
        ident_name(name)
    ));

    for member in &s.members {
        if !member.docs.is_empty() {
            ret.push_str("    /**\n");
            for line in member.docs.lines() {
                ret.push_str(&format!("     * {}\n", line));
            }
            ret.push_str("     */\n");
        }
        ret.push_str(&format!(
            "    {} {};\n",
            typeref_name(&member.tref),
            ident_name(&member.name)
        ));
        ret.push_str("\n");
    }

    ret.push_str(&format!("}} __wasi_{}_t;\n", ident_name(name)));
    ret.push_str("\n");
    /*
        ret.push_str(&format!(
            "_Static_assert(sizeof(__wasi_{}_t) == {}, \"witx calculated size\");\n",
            ident_name(name),
            s.mem_size()
        ));
        ret.push_str(&format!(
            "_Static_assert(_Alignof(__wasi_{}_t) == {}, \"witx calculated align\");\n",
            ident_name(name),
            s.mem_align()
        ));

        for layout in s.member_layout() {
            ret.push_str(&format!(
                "_Static_assert(offsetof(__wasi_{}_t, {}) == {}, \"witx calculated offset\");\n",
                ident_name(name),
                ident_name(&layout.member.name),
                layout.offset
            ));
        }
    */
    ret.push_str("\n");
}

fn print_variant(ret: &mut String, name: &Id, v: &Variant) {
    if v.cases.iter().all(|v| v.tref.is_none()) {
        return print_enum(ret, name, v);
    }
    ret.push_str(&format!(
        "typedef union __wasi_{}_u_t {{\n",
        ident_name(name)
    ));

    for case in &v.cases {
        if let Some(tref) = &case.tref {
            if !case.docs.is_empty() {
                ret.push_str("    /**\n");
                for line in case.docs.lines() {
                    ret.push_str(&format!("    * {}\n", line));
                }
                ret.push_str("    */\n");
            }
            ret.push_str(&format!(
                "    {} {};\n",
                typeref_name(tref),
                ident_name(&case.name)
            ));
        }
    }
    ret.push_str(&format!("}} __wasi_{}_u_t;\n", ident_name(name)));

    ret.push_str(&format!(
        "typedef struct __wasi_{}_t {{\n",
        ident_name(name)
    ));

    ret.push_str(&format!("    {} tag;\n", intrepr_name(v.tag_repr)));
    ret.push_str(&format!("    __wasi_{}_u_t u;\n", ident_name(name)));

    ret.push_str(&format!("}} __wasi_{}_t;\n", ident_name(name)));
    ret.push_str("\n");
    /*
        ret.push_str(&format!(
            "_Static_assert(sizeof(__wasi_{}_t) == {}, \"witx calculated size\");\n",
            ident_name(name),
            v.mem_size()
        ));
        ret.push_str(&format!(
            "_Static_assert(_Alignof(__wasi_{}_t) == {}, \"witx calculated align\");\n",
            ident_name(name),
            v.mem_align()
        ));
    */
    ret.push_str("\n");
}

fn print_handle(ret: &mut String, name: &Id, _h: &HandleDatatype) {
    ret.push_str(&format!("typedef int __wasi_{}_t;\n\n", ident_name(name)));
    /*
        ret.push_str(&format!(
            "_Static_assert(sizeof(__wasi_{}_t) == {}, \"witx calculated size\");\n",
            ident_name(name),
            h.mem_size()
        ));
        ret.push_str(&format!(
            "_Static_assert(_Alignof(__wasi_{}_t) == {}, \"witx calculated align\");\n",
            ident_name(name),
            h.mem_align()
        ));
    */
    ret.push_str("\n");
}

fn print_module(header: &mut String, source: &mut String, m: &Module) {
    header.push_str("/**\n");
    header.push_str(&format!(" * @defgroup {}\n", ident_name(&m.name),));
    for line in m.docs.lines() {
        header.push_str(&format!(" * {}\n", line));
    }
    header.push_str(" * @{\n");
    header.push_str(" */\n");
    header.push_str("\n");

    for func in m.funcs() {
        print_func_header(header, &func);
        print_func_source(source, &func, &m.name);
    }

    header.push_str("/** @} */\n");
    header.push_str("\n");
}

fn print_func_header(ret: &mut String, func: &InterfaceFunc) {
    if !func.docs.is_empty() {
        ret.push_str("/**\n");
        for line in func.docs.lines() {
            ret.push_str(&format!(" * {}\n", line));
        }
        if !func.results.is_empty() {
            let first_result = &func.results[0];
            if !first_result.docs.is_empty() {
                ret.push_str(" * @return\n");
                for line in first_result.docs.lines() {
                    ret.push_str(&format!(" * {}\n", line));
                }
            }
        }
        ret.push_str(" */\n");
    }

    print_func_signature(ret, func, true);
    ret.push_str(" __WASI_NOEXCEPT ");
    if func.results.len() > 0 {
        ret.push_str(" __attribute__((__warn_unused_result__))");
    }

    ret.push_str(";\n");
}

fn print_func_signature(ret: &mut String, func: &InterfaceFunc, header: bool) {
    let mut params = Vec::new();
    for param in func.params.iter() {
        add_params(
            &mut params,
            &ident_name(&param.name),
            &param.tref,
            &param.docs,
        );
    }
    match func.results.len() {
        0 => {
            if func.noreturn {
                ret.push_str("_Noreturn ");
            }
            ret.push_str("void ");
        }
        1 => {
            assert!(!func.noreturn);
            push_return_type(ret, &mut params, &func.results[0].tref);
        }
        _ => panic!("unsupported number of return values"),
    }

    ret.push_str(&format!("__wasi_{}(\n", ident_name(&func.name)));
    if params.len() == 0 {
        ret.push_str("    void\n");
    } else {
        for (i, (docs, param)) in params.iter().enumerate() {
            if header {
                if let Some(docs) = docs {
                    ret.push_str("    /**\n");
                    for line in docs.lines() {
                        ret.push_str(&format!("     * {}\n", line));
                    }
                    ret.push_str("     */\n");
                }
            }
            ret.push_str("    ");
            ret.push_str(&param);
            if i != params.len() - 1 {
                ret.push_str(",");
            }
            ret.push_str("\n");
        }
    }
    ret.push_str(")");
}

fn c_typeref_name(tref: &TypeRef, wtp: &witx::WasmType) -> String {
    match tref {
        TypeRef::Name(_) => wasm_type(wtp).to_string(),
        TypeRef::Value(anon_type) => match &**anon_type {
            Type::List(_) => unreachable!("arrays excluded above"),
            Type::Builtin(b) => builtin_type_name(*b).to_string(),
            Type::Pointer(_) => "intptr_t".to_string(),
            Type::ConstPointer(_) => "intptr_t".to_string(),
            Type::Record { .. } | Type::Variant { .. } | Type::Handle { .. } => unreachable!(
                "wasi should not have anonymous structs, unions, enums, flags, handles"
            ),
        },
    }
}

fn print_c_typeref_casting_names(
    ret: &mut String,
    func: &InterfaceFunc,
    signparams: &std::vec::Vec<witx::WasmType>,
    casting: bool,
) {
    let mut iszero = true;
    let mut j = 0;
    for param in func.params.iter() {
        if iszero {
            iszero = false
        } else {
            ret.push_str(", ");
        }
        let identifiername = ident_name(&param.name);
        match &**param.tref.type_() {
            Type::List(_) => {
                if casting {
                    ret.push_str(&format!(
                        "(intptr_t) {0}, (__wasi_size_t) {0}_len",
                        &identifiername
                    ));
                } else {
                    ret.push_str("intptr_t, __wasi_size_t");
                }
                j = j + 2;
            }
            _ => {
                // Fix around buf_len which is incorrectly marked uint32_t in metadata
                if identifiername.ends_with("_len") {
                    if casting {
                        ret.push_str(&format!("(__wasi_size_t) {}", &identifiername));
                    } else {
                        ret.push_str("__wasi_size_t");
                    }
                } else {
                    if casting {
                        ret.push_str(&format!(
                            "({}) {}",
                            c_typeref_name(&param.tref, &signparams[j]),
                            &identifiername
                        ));
                    } else {
                        ret.push_str(&c_typeref_name(&param.tref, &signparams[j]));
                    }
                }
                j = j + 1;
            }
        }
    }
    match func.results.len() {
        0 => {}
        1 => {
            assert!(!func.noreturn);

            match &**func.results[0].tref.type_() {
                Type::Variant(v) => {
                    let _err = match &v.cases[1].tref {
                        Some(ty) => ty,
                        None => panic!("unsupported type as a return value"),
                    };
                    let ok = match &v.cases[0].tref {
                        Some(ty) => ty,
                        None => return,
                    };
                    match &**ok.type_() {
                        Type::Record(r) if r.is_tuple() => {
                            for (i, _) in r.members.iter().enumerate() {
                                if iszero {
                                    iszero = false
                                } else {
                                    ret.push_str(", ");
                                }
                                if casting {
                                    ret.push_str(&format!("(intptr_t) retptr{}", i));
                                } else {
                                    ret.push_str("intptr_t");
                                }
                            }
                        }
                        _ => {
                            if !iszero {
                                ret.push_str(", ");
                            }
                            if casting {
                                ret.push_str("(intptr_t) retptr0");
                            } else {
                                ret.push_str("intptr_t");
                            }
                        }
                    }
                }
                _ => panic!("unsupported type as a return value"),
            }
        }
        _ => panic!("unsupported number of return values"),
    }
}

fn print_c_typeref_names(
    ret: &mut String,
    func: &InterfaceFunc,
    signparams: &std::vec::Vec<witx::WasmType>,
) {
    (print_c_typeref_casting_names(ret, func, signparams, false));
}

fn print_c_casting_names(
    ret: &mut String,
    func: &InterfaceFunc,
    signparams: &std::vec::Vec<witx::WasmType>,
) {
    (print_c_typeref_casting_names(ret, func, signparams, true));
}

fn print_func_source(ret: &mut String, func: &InterfaceFunc, module_name: &Id) {
    let (params, results) = func.wasm_signature();

    if func.noreturn {
        ret.push_str("_Noreturn ");
    }
    match results.len() {
        0 => ret.push_str("void "),
        1 => {
            ret.push_str(wasm_type(&results[0]));
            ret.push_str(" ");
        }
        _ => unimplemented!(),
    }

    ret.push_str("__imported_");
    ret.push_str(&ident_name(module_name));
    ret.push_str("_");
    ret.push_str(&ident_name(&func.name));
    ret.push_str("(");
    print_c_typeref_names(ret, func, &params);

    ret.push_str(") __WASI_NOEXCEPT __attribute__((\n");
    ret.push_str(&format!(
        "    __import_module__(\"{}\"),\n",
        ident_name(module_name)
    ));
    ret.push_str(&format!(
        "    IMPORT_NAME(\"{}\")\n",
        ident_name(&func.name)
    ));
    ret.push_str("));\n\n");
    print_func_signature(ret, func, false);
    ret.push_str("{\n");

    // Any string arguments will have their length passed to the raw wasi API,
    // but we only took a `char*` as a parameter. Get the length of all string
    // arguments here for it to get passed along later.
    for param in func.params.iter() {
        if let Type::List(ty) = &**param.tref.type_() {
            if let Type::Builtin(BuiltinType::Char) = &**ty.type_() {
                ret.push_str(&format!(
                    "    size_t {}_len = strlen({0});\n",
                    ident_name(&param.name)
                ));
            }
        }
    }
    ret.push_str("    ");
    if !func.noreturn {
        ret.push_str("return (__wasi_errno_t) ");
    }
    ret.push_str("__imported_");
    ret.push_str(&ident_name(module_name));
    ret.push_str("_");
    ret.push_str(&ident_name(&func.name));
    ret.push_str("(");
    print_c_casting_names(ret, func, &params);
    ret.push_str(");\n}\n\n");
}

fn push_return_type(ret: &mut String, params: &mut Vec<(Option<String>, String)>, tref: &TypeRef) {
    match &**tref.type_() {
        Type::Variant(v) => {
            assert_eq!(v.cases.len(), 2, "unsupported type as a return value");
            assert!(
                v.cases[0].name == "ok",
                "unsupported type as a return value"
            );
            assert!(
                v.cases[1].name == "err",
                "unsupported type as a return value"
            );
            let err = match &v.cases[1].tref {
                Some(ty) => ty,
                None => panic!("unsupported type as a return value"),
            };
            ret.push_str(&typeref_name(&err));
            ret.push_str(" ");

            let ok = match &v.cases[0].tref {
                Some(ty) => ty,
                None => return,
            };
            match &**ok.type_() {
                Type::Record(r) if r.is_tuple() => {
                    for (i, member) in r.members.iter().enumerate() {
                        params.push((None, format!("{} *retptr{}", typeref_name(&member.tref), i)));
                    }
                }
                _ => {
                    params.push((None, format!("{} *retptr0", typeref_name(ok))));
                }
            }
        }
        _ => panic!("unsupported type as a return value"),
    }
}

fn add_params(params: &mut Vec<(Option<String>, String)>, name: &str, tref: &TypeRef, docs: &str) {
    let docs = if docs.is_empty() {
        None
    } else {
        Some(docs.to_string())
    };
    match &**tref.type_() {
        Type::List(element) => match &**element.type_() {
            Type::Builtin(BuiltinType::Char) => {
                params.push((docs, format!("char const *{}", name)));
            }
            _ => {
                params.push((docs, format!("{} const *{}", typeref_name(&element), name)));
                params.push((
                    Some(format!("The length of the array pointed to by `{}`.", name,)),
                    format!("size_t {}_len", name),
                ));
            }
        },
        _ => {
            params.push((docs, format!("{} {}", typeref_name(tref), name)));
        }
    }
}

fn ident_name(i: &Id) -> String {
    i.as_str().to_string()
}

fn builtin_type_name(b: BuiltinType) -> &'static str {
    match b {
        BuiltinType::U8 { lang_c_char: true } => {
            panic!("no type name for string or char8 builtins")
        }
        BuiltinType::U8 { lang_c_char: false } => "uint8_t",
        BuiltinType::U16 => "uint16_t",
        BuiltinType::U32 {
            lang_ptr_size: true,
        } => "size_t",
        BuiltinType::U32 {
            lang_ptr_size: false,
        } => "uint32_t",
        BuiltinType::U64 => "uint64_t",
        BuiltinType::S8 => "int8_t",
        BuiltinType::S16 => "int16_t",
        BuiltinType::S32 => "int32_t",
        BuiltinType::S64 => "int64_t",
        BuiltinType::F32 => "float",
        BuiltinType::F64 => "double",
        BuiltinType::Char => "char32_t",
    }
}

fn typeref_name(tref: &TypeRef) -> String {
    match &**tref.type_() {
        Type::Builtin(BuiltinType::U8 { lang_c_char: true }) | Type::List(_) => {
            panic!("unsupported grammar: cannot construct name of string or array",)
        }
        _ => {}
    }

    match tref {
        TypeRef::Name(named_type) => namedtype_name(&named_type),
        TypeRef::Value(anon_type) => match &**anon_type {
            Type::List(_) => unreachable!("arrays excluded above"),
            Type::Builtin(b) => builtin_type_name(*b).to_string(),
            Type::Pointer(p) => format!("{} *", typeref_name(&*p)),
            Type::ConstPointer(p) => format!("{} const *", typeref_name(&*p)),
            Type::Record { .. } | Type::Variant { .. } | Type::Handle { .. } => unreachable!(
                "wasi should not have anonymous structs, unions, enums, flags, handles"
            ),
        },
    }
}

fn namedtype_name(named_type: &NamedType) -> String {
    match &**named_type.type_() {
        Type::Pointer(p) => format!("{} *", typeref_name(&*p)),
        Type::ConstPointer(p) => format!("{} const *", typeref_name(&*p)),
        Type::List(_) => unreachable!("arrays excluded above"),
        _ => format!("__wasi_{}_t", named_type.name.as_str()),
    }
}

fn intrepr_name(i: IntRepr) -> &'static str {
    match i {
        IntRepr::U8 => "uint8_t",
        IntRepr::U16 => "uint16_t",
        IntRepr::U32 => "uint32_t",
        IntRepr::U64 => "uint64_t",
    }
}

fn intrepr_const(i: IntRepr) -> &'static str {
    match i {
        IntRepr::U8 => "UINT8_C",
        IntRepr::U16 => "UINT16_C",
        IntRepr::U32 => "UINT32_C",
        IntRepr::U64 => "UINT64_C",
    }
}

fn wasm_type(wasm: &WasmType) -> &'static str {
    match wasm {
        WasmType::I32 => "int32_t",
        WasmType::I64 => "int64_t",
        WasmType::F32 => "float",
        WasmType::F64 => "double",
    }
}
