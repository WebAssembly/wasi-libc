use heck::ShoutySnakeCase;
use witx::*;

pub(crate) fn to_c_header(doc: &Document, inputs_str: &str) -> String {
    let mut ret = String::new();

    ret.push_str(&format!(
        r#"/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   {}
 *
 * @file
 * This file describes the [WASI] interface, consisting of functions, types,
 * and defined values (macros).
 *
 * The interface described here is greatly inspired by [CloudABI]'s clean,
 * thoughtfully-designed, cabability-oriented, POSIX-style API.
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

#ifdef __cplusplus
extern "C" {{
#endif

// TODO: Encoding this in witx.
#define __WASI_DIRCOOKIE_START (UINT64_C(0))
"#,
        inputs_str,
    ));

    for nt in doc.typenames() {
        print_datatype(&mut ret, &*nt);
    }

    for m in doc.modules() {
        print_module(&mut ret, &m);
    }

    ret.push_str(
        r#"#ifdef __cplusplus
}
#endif

#endif
"#,
    );

    ret
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
            Type::Enum(e) => print_enum(ret, &nt.name, e),
            Type::Int(i) => print_int(ret, &nt.name, i),
            Type::Flags(f) => print_flags(ret, &nt.name, f),
            Type::Struct(s) => print_struct(ret, &nt.name, s),
            Type::Union(u) => print_union(ret, &nt.name, u),
            Type::Handle(h) => print_handle(ret, &nt.name, h),
            Type::Builtin { .. }
            | Type::Array { .. }
            | Type::Pointer { .. }
            | Type::ConstPointer { .. } => print_alias(ret, &nt.name, &nt.tref),
        },
        TypeRef::Name(_) => print_alias(ret, &nt.name, &nt.tref),
    }
}

fn print_alias(ret: &mut String, name: &Id, dest: &TypeRef) {
    match &*dest.type_() {
        Type::Array(_) => {
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
        }
    }
}

fn print_enum(ret: &mut String, name: &Id, e: &EnumDatatype) {
    ret.push_str(&format!(
        "typedef {} __wasi_{}_t;\n",
        intrepr_name(e.repr),
        ident_name(name)
    ));
    ret.push_str("\n");

    for (index, variant) in e.variants.iter().enumerate() {
        if !variant.docs.is_empty() {
            ret.push_str("/**\n");
            for line in variant.docs.lines() {
                ret.push_str(&format!(" * {}\n", line));
            }
            ret.push_str(" */\n");
        }
        ret.push_str(&format!(
            "#define __WASI_{}_{} ({}({}))\n",
            ident_name(&name).to_shouty_snake_case(),
            ident_name(&variant.name).to_shouty_snake_case(),
            intrepr_const(e.repr),
            index
        ));
        ret.push_str("\n");
    }
}

fn print_int(ret: &mut String, name: &Id, i: &IntDatatype) {
    ret.push_str(&format!(
        "typedef {} __wasi_{}_t;\n",
        intrepr_name(i.repr),
        ident_name(name)
    ));
    ret.push_str("\n");

    for (index, const_) in i.consts.iter().enumerate() {
        if !const_.docs.is_empty() {
            ret.push_str("/**\n");
            for line in const_.docs.lines() {
                ret.push_str(&format!(" * {}\n", line));
            }
            ret.push_str(" */\n");
        }
        ret.push_str(&format!(
            "#define __WASI_{}_{} ({}({}))\n",
            ident_name(&name).to_shouty_snake_case(),
            ident_name(&const_.name).to_shouty_snake_case(),
            intrepr_const(i.repr),
            index
        ));
        ret.push_str("\n");
    }

    ret.push_str(&format!(
        "_Static_assert(sizeof(__wasi_{}_t) == {}, \"witx calculated size\");\n",
        ident_name(name),
        i.repr.mem_size()
    ));
    ret.push_str(&format!(
        "_Static_assert(_Alignof(__wasi_{}_t) == {}, \"witx calculated align\");\n",
        ident_name(name),
        i.repr.mem_align()
    ));

    ret.push_str("\n");
}

fn print_flags(ret: &mut String, name: &Id, f: &FlagsDatatype) {
    ret.push_str(&format!(
        "typedef {} __wasi_{}_t;\n",
        intrepr_name(f.repr),
        ident_name(name)
    ));
    ret.push_str("\n");

    for (index, flag) in f.flags.iter().enumerate() {
        if !flag.docs.is_empty() {
            ret.push_str("/**\n");
            for line in flag.docs.lines() {
                ret.push_str(&format!(" * {}\n", line));
            }
            ret.push_str(" */\n");
        }
        ret.push_str(&format!(
            "#define __WASI_{}_{} ({}({}))\n",
            ident_name(name).to_shouty_snake_case(),
            ident_name(&flag.name).to_shouty_snake_case(),
            intrepr_const(f.repr),
            1u128 << index
        ));
        ret.push_str("\n");
    }
}

fn print_struct(ret: &mut String, name: &Id, s: &StructDatatype) {
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
}

fn print_union(ret: &mut String, name: &Id, u: &UnionDatatype) {
    ret.push_str(&format!("typedef union __wasi_{}_t {{\n", ident_name(name)));

    for variant in &u.variants {
        if !variant.docs.is_empty() {
            ret.push_str("    /**\n");
            for line in variant.docs.lines() {
                ret.push_str(&format!("     * {}\n", line));
            }
            ret.push_str("     */\n");
        }
        ret.push_str(&format!(
            "    {} {};\n",
            typeref_name(&variant.tref),
            ident_name(&variant.name)
        ));
        ret.push_str("\n");
    }

    ret.push_str(&format!("}} __wasi_{}_t;\n", ident_name(name)));
    ret.push_str("\n");
}

fn print_handle(ret: &mut String, name: &Id, _h: &HandleDatatype) {
    ret.push_str(&format!("typedef int __wasi_{}_t;", ident_name(name)));
}

fn print_module(ret: &mut String, m: &Module) {
    ret.push_str("/**\n");
    ret.push_str(&format!(" * @defgroup {}\n", ident_name(&m.name),));
    for line in m.docs.lines() {
        ret.push_str(&format!(" * {}\n", line));
    }
    ret.push_str(" * @{\n");
    ret.push_str(" */\n");
    ret.push_str("\n");

    for func in m.funcs() {
        print_func(ret, &func, &m.name);
    }

    ret.push_str("/** @} */\n");
    ret.push_str("\n");
}

fn print_func(ret: &mut String, func: &InterfaceFunc, module_name: &Id) {
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
                    ret.push_str(&format!(" * {}", line));
                }
            }
        }
        ret.push_str(" */\n");
    }
    if func.results.is_empty() {
        // Special-case "proc_exit" as _Noreturn -- TODO: Encode this in witx.
        if func.name.as_str() == "proc_exit" {
            ret.push_str("_Noreturn ");
        }
        ret.push_str("void ");
    } else {
        let first_result = &func.results[0];
        ret.push_str(&format!("{} ", typeref_name(&first_result.tref)));
    }

    ret.push_str(&format!("__wasi_{}(\n", ident_name(&func.name)));

    if func.params.is_empty() && func.results.len() <= 1 {
        ret.push_str("    void\n");
    }
    for (index, param) in func.params.iter().enumerate() {
        if !param.docs.is_empty() {
            ret.push_str("    /**\n");
            for line in param.docs.lines() {
                ret.push_str(&format!("     * {}\n", line));
            }
            ret.push_str("     */\n");
        }
        add_params(ret, &ident_name(&param.name), &param.tref);
        ret.push_str(&format!(
            "{}\n",
            if index + 1 < func.params.len() || func.results.len() > 1 {
                ",\n"
            } else {
                ""
            }
        ));
    }

    for (index, result) in func.results.iter().enumerate() {
        if index == 0 {
            // The first result is returned by value above.
            continue;
        }
        if !result.docs.is_empty() {
            ret.push_str("    /**\n");
            for line in result.docs.lines() {
                ret.push_str(&format!("     * {}\n", line));
            }
            ret.push_str("     */\n");
        }
        ret.push_str(&format!(
            "    {} *{}{}\n",
            typeref_name(&result.tref),
            ident_name(&result.name),
            if index + 1 < func.results.len() {
                ","
            } else {
                ""
            }
        ));
    }

    ret.push_str(") __attribute__((\n");
    ret.push_str(&format!(
        "    __import_module__(\"{}\"),\n",
        ident_name(module_name)
    ));
    ret.push_str(&format!(
        "    __import_name__(\"{}\")",
        ident_name(&func.name)
    ));
    if !func.results.is_empty() {
        ret.push_str(",\n    __warn_unused_result__\n");
    }
    ret.push_str("));\n");
    ret.push_str("\n");
}

fn add_params(ret: &mut String, name: &str, tref: &TypeRef) {
    match &*tref.type_() {
        Type::Builtin(BuiltinType::String) => {
            ret.push_str(&format!("    const char *{},\n", name));
            ret.push_str("\n");
            ret.push_str("    /**\n");
            ret.push_str(&format!(
                "     * The length of the buffer pointed to by `{}`.\n",
                name,
            ));
            ret.push_str("     */\n");
            ret.push_str(&format!("    size_t {}_len", name));
        }
        Type::Array(element) => {
            ret.push_str(&format!(
                "    const {} *{},\n",
                typeref_name(&element),
                name
            ));
            ret.push_str("\n");
            ret.push_str("    /**\n");
            ret.push_str(&format!(
                "     * The length of the array pointed to by `{}`.\n",
                name,
            ));
            ret.push_str("     */\n");
            ret.push_str(&format!("    size_t {}_len", name));
        }
        _ => {
            ret.push_str(&format!("    {} {}", typeref_name(tref), name));
        }
    }
}

fn ident_name(i: &Id) -> String {
    i.as_str().to_string()
}

fn builtin_type_name(b: BuiltinType) -> &'static str {
    match b {
        BuiltinType::String | BuiltinType::Char8 => {
            panic!("no type name for string or char8 builtins")
        }
        BuiltinType::USize => "size_t",
        BuiltinType::U8 => "uint8_t",
        BuiltinType::U16 => "uint16_t",
        BuiltinType::U32 => "uint32_t",
        BuiltinType::U64 => "uint64_t",
        BuiltinType::S8 => "int8_t",
        BuiltinType::S16 => "int16_t",
        BuiltinType::S32 => "int32_t",
        BuiltinType::S64 => "int64_t",
        BuiltinType::F32 => "float",
        BuiltinType::F64 => "double",
    }
}

fn typeref_name(tref: &TypeRef) -> String {
    match &*tref.type_() {
        Type::Builtin(BuiltinType::String) | Type::Builtin(BuiltinType::Char8) | Type::Array(_) => {
            panic!("unsupported grammar: cannot construct name of string or array",)
        }
        _ => {}
    }

    match tref {
        TypeRef::Name(named_type) => match &*named_type.type_() {
            Type::Pointer(p) => format!("{} *", typeref_name(&*p)),
            Type::ConstPointer(p) => format!("const {} *", typeref_name(&*p)),
            Type::Array(_) => unreachable!("arrays excluded above"),
            _ => format!("__wasi_{}_t", named_type.name.as_str()),
        },
        TypeRef::Value(anon_type) => match &**anon_type {
            Type::Array(_) => unreachable!("arrays excluded above"),
            Type::Builtin(b) => builtin_type_name(*b).to_string(),
            Type::Pointer(p) => format!("{} *", typeref_name(&*p)),
            Type::ConstPointer(p) => format!("const {} *", typeref_name(&*p)),
            Type::Int(i) => format!("{}", intrepr_name(i.repr)),
            Type::Struct { .. }
            | Type::Union { .. }
            | Type::Enum { .. }
            | Type::Flags { .. }
            | Type::Handle { .. } => unreachable!(
                "wasi should not have anonymous structs, unions, enums, flags, handles"
            ),
        },
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
