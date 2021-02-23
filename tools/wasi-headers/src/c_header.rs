use heck::ShoutySnakeCase;
use std::collections::HashMap;
use std::mem;
use witx::*;

pub struct Generated {
    pub header: String,
    pub source: String,
}

pub fn to_c(doc: &Document, inputs_str: &str) -> Generated {
    let mut header = String::new();

    header.push_str(&format!(
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

#ifndef __wasm32__
#error <wasi/api.h> only supports wasm32; doesn't yet support wasm64
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
_Static_assert(_Alignof(void*) == 4, "non-wasi data layout");

#ifdef __cplusplus
extern "C" {{
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
        r#"#ifdef __cplusplus
}
#endif

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

    ret.push_str("\n");
}

fn print_handle(ret: &mut String, name: &Id, h: &HandleDatatype) {
    ret.push_str(&format!("typedef int __wasi_{}_t;\n\n", ident_name(name)));

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
    for (i, param) in params.iter().enumerate() {
        if i > 0 {
            ret.push_str(", ");
        }
        ret.push_str(wasm_type(param));
        ret.push_str(&format!(" arg{}", i));
    }
    ret.push_str(") __attribute__((\n");
    ret.push_str(&format!(
        "    __import_module__(\"{}\"),\n",
        ident_name(module_name)
    ));
    ret.push_str(&format!(
        "    __import_name__(\"{}\")\n",
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

    func.call_wasm(
        module_name,
        &mut C {
            src: ret,
            params: &func.params,
            block_storage: Vec::new(),
            blocks: Vec::new(),
        },
    );

    ret.push_str("}\n\n");

    /// This is a structure which implements the glue necessary to translate
    /// between the C API of a function and the desired WASI ABI we're being
    /// told it has.
    ///
    /// It's worth nothing that this will, in the long run, get much fancier.
    /// For now this is extremely simple and entirely assumes that the WASI ABI
    /// matches our C ABI. This means it will only really generate valid code
    /// as-is *today* and won't work for any updates to the WASI ABI in the
    /// future.
    ///
    /// It's hoped that this situation will improve as interface types and witx
    /// continue to evolve and there's a more clear path forward for how to
    /// translate an interface types signature to a C API.
    struct C<'a> {
        src: &'a mut String,
        params: &'a [InterfaceFuncParam],
        block_storage: Vec<String>,
        blocks: Vec<String>,
    }

    impl Bindgen for C<'_> {
        type Operand = String;

        fn push_block(&mut self) {
            let prev = mem::replace(self.src, String::new());
            self.block_storage.push(prev);
        }

        fn finish_block(&mut self, operand: Option<String>) {
            let to_restore = self.block_storage.pop().unwrap();
            let src = mem::replace(self.src, to_restore);
            assert!(src.is_empty());
            match operand {
                None => self.blocks.push(String::new()),
                Some(s) => self.blocks.push(s),
            }
        }

        fn allocate_space(&mut self, _: usize, _: &NamedType) {
            // not necessary due to us taking parameters as pointers
        }

        fn emit(
            &mut self,
            inst: &Instruction<'_>,
            operands: &mut Vec<String>,
            results: &mut Vec<String>,
        ) {
            let mut top_as = |cvt: &str| {
                let s = operands.pop().unwrap();
                results.push(format!("({}) {}", cvt, s));
            };

            match inst {
                Instruction::GetArg { nth } => {
                    results.push(ident_name(&self.params[*nth].name));
                }
                // For the C bindings right now any parameter which needs its
                // address taken is already taken as a pointer, so we can just
                // forward the operand to the result.
                Instruction::AddrOf => results.push(operands.pop().unwrap()),

                Instruction::I64FromU64 => top_as("int64_t"),
                Instruction::I32FromPointer
                | Instruction::I32FromConstPointer
                | Instruction::I32FromHandle { .. }
                | Instruction::I32FromUsize
                | Instruction::I32FromChar
                | Instruction::I32FromU8
                | Instruction::I32FromS8
                | Instruction::I32FromChar8
                | Instruction::I32FromU16
                | Instruction::I32FromS16
                | Instruction::I32FromU32 => top_as("int32_t"),

                Instruction::I32FromBitflags { .. } | Instruction::I64FromBitflags { .. } => {
                    // Rely on C's casting for this
                    results.push(operands.pop().unwrap());
                }

                // No conversion necessary
                Instruction::F32FromIf32
                | Instruction::F64FromIf64
                | Instruction::If32FromF32
                | Instruction::If64FromF64
                | Instruction::I64FromS64
                | Instruction::I32FromS32 => results.push(operands.pop().unwrap()),

                Instruction::ListPointerLength => {
                    let list = operands.pop().unwrap();
                    results.push(format!("(int32_t) {}", list));
                    results.push(format!("(int32_t) {}_len", list));
                }
                Instruction::ReturnPointerGet { n } => {
                    // We currently match the wasi ABI with the actual
                    // function's API signature in C, this means when a return
                    // pointer is asked for we can simply forward our parameter
                    // that's a return pointer.
                    results.push(format!("(int32_t) retptr{}", n));
                }

                Instruction::Load { .. } => {
                    // this is currently skipped because return pointers are
                    // already evident in parameters and so we don't need to
                    // load/store from them again
                    results.push("dummy".to_string());
                }

                Instruction::ReuseReturn => {
                    results.push("ret".to_string());
                }

                Instruction::TupleLift { .. } => {
                    // this is currently skipped because tuples are only used
                    // as return values and those are always returned through
                    // out-ptrs, so the values have already been passed through
                    // at this point.
                    results.push("dummy".to_string());
                }

                Instruction::ResultLift => {
                    let err = self.blocks.pop().unwrap();

                    // Our ok block should either have done nothing (meaning
                    // there's no "ok" payload) or it loaded a return pointer
                    // and/or tuple some values. In all these cases we discard
                    // the results since out pointers have already been written
                    // to and we only return the discriminant from the
                    // function.
                    let ok = self.blocks.pop().unwrap();
                    assert!(ok == "dummy" || ok == "");

                    // Note that we just push the result of the error block.
                    // Our block management asserts that it's an expression,
                    // which in this case will basically always be casting the
                    // error code to an error code variant, which we return.
                    results.push(err);
                }

                // Enums are represented in C simply as the integral tag type
                Instruction::EnumLift { ty } => match &**ty.type_() {
                    Type::Variant(v) => top_as(intrepr_name(v.tag_repr)),
                    _ => unreachable!(),
                },
                Instruction::EnumLower { .. } => top_as("int32_t"),

                Instruction::CallWasm {
                    module,
                    name,
                    params: _,
                    results: func_results,
                } => {
                    assert!(func_results.len() < 2);
                    self.src.push_str("    ");
                    if func_results.len() > 0 {
                        self.src.push_str(wasm_type(&func_results[0]));
                        self.src.push_str(" ret = ");
                        results.push("ret".to_string());
                    }
                    self.src.push_str("__imported_");
                    self.src.push_str(module);
                    self.src.push_str("_");
                    self.src.push_str(name);
                    self.src.push_str("(");
                    self.src.push_str(&operands.join(", "));
                    self.src.push_str(");\n");
                }

                Instruction::Return { amt: 0 } => {}
                Instruction::Return { amt: 1 } => {
                    self.src.push_str("    return ");
                    self.src.push_str(&operands[0]);
                    self.src.push_str(";\n");
                }

                other => panic!("unimplemented instruction {:?}", other),
            }
        }
    }
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
                params.push((docs, format!("const char *{}", name)));
            }
            _ => {
                params.push((docs, format!("const {} *{}", typeref_name(&element), name)));
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
            Type::ConstPointer(p) => format!("const {} *", typeref_name(&*p)),
            Type::Record { .. } | Type::Variant { .. } | Type::Handle { .. } => unreachable!(
                "wasi should not have anonymous structs, unions, enums, flags, handles"
            ),
        },
    }
}

fn namedtype_name(named_type: &NamedType) -> String {
    match &**named_type.type_() {
        Type::Pointer(p) => format!("{} *", typeref_name(&*p)),
        Type::ConstPointer(p) => format!("const {} *", typeref_name(&*p)),
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
