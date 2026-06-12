# WASI libc "top half".

The WASI libc "top half" is conceptually the upper half of a traditional libc
implementation, consisting of C standard library and other relatively
higher-level functionality.

Code in the musl directory is based on musl revision
040c1d16b468c50c04fc94edff521f1637708328, which is v1.2.0, from
git://git.musl-libc.org/musl.

Whole files which are unused are omitted. Changes to upstream code are wrapped
in preprocessor directives controlled by the macro `__wasilibc_unmodified_upstream`.

Some major known missing areas include:
 - threads
 - aio
 - setjmp
 - signals
 - ipc
 - termios
 - nss
 - timezones
 - non-builtin locales
 - TIOCGWINSZ (because cloudabi lacks it; affects isatty, line buffering for stdout)
 - O\_CLOEXEC, O\_NOCTTY (because cloudabi lacks them)

## Updating Musl

The `libc-top-half/musl` directory is a `git subtree` for the upstream musl
repository. Updating it can be done with:

```
git subtree pull \
    --prefix libc-top-half/musl \
    https://git.musl-libc.org/git/musl v1.2.6 \
    --squash
```

Resolve all conflicts and such that come about from the merge. Track deleted and
updated files and update `libc-top-half/CMakeLists.txt` as appropriate.
