Code in the musl directory is based on musl revision
e97681d6f2c44bf5fa9ecdd30607cb63c780062e, which is v1.1.22, from
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
 - environment variables
 - timezones
 - non-builtin locales
 - TIOCGWINSZ (because cloudabi lacks it; affects isatty, line buffering for stdout)
 - O\_CLOEXEC, O\_NOCTTY (because cloudabi lacks them)
