This code is based on musl revision 1691b23955590d1eb66a11158fdd91c86337e886
from git://git.musl-libc.org/musl.

Whole files which are unused are omitted. Changes to upstream code are wrapped
in preprocessor directives controlled by the macro `__wasilibc_unmodified_upstream__`.

Some major known missing areas include:
 - threads
 - setjmp
 - signals
 - ipc
 - getenv
 - TIOCGWINSZ (because cloudabi lacks it; affects isatty, line buffering for stdout)
 - O\_CLOEXEC, O\_NOCTTY (because cloudabi lacks them)
