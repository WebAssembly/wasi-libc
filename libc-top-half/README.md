Code in the musl directory is based on musl revision
b07d45eb01e900f0176894fdedab62285f5cb8be, which is v1.1.23, from
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
