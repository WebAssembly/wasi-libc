#!/usr/bin/env python

# Find additional compilation flags specified in test files.
#
# This script accepts a single file as an argument and looks for a comment like
# the following: `// add-flags.py(<NAME>): <FLAGS>`. If found, the `<FLAGS>` are
# printed to stdout.
#
# Example:
# ```
# $ head one.c
# //! add-flags.py(CFLAGS): -DFOO
# $ ./add-flags.py CFLAGS one.c
# -DFOO

import sys
import os
import re
import logging

"""
Match a C comment like the following: `//! add-flags.py: <FLAGS>`.
"""
PATTERN = re.compile('\\s*//\\!\\s*add-flags\\.py\\(([^)]+)\\):\\s*(.*)')


def find_flags(name, file):
    with open(file, 'r') as f:
        for lineno, line in enumerate(f, start=1):
            match = PATTERN.match(line)
            if match and match[1] == name:
                pos = f'[{file}:{lineno}]'
                logging.debug(f'{pos} found flags')
                return match[2].strip()


def main(name, file):
    flags = find_flags(name, file)
    if flags:
        print(flags)


if __name__ == "__main__":
    logging.getLogger().name = os.path.basename(__file__)
    if os.environ.get('VERBOSE'):
        logging.basicConfig(level=logging.DEBUG)
    if len(sys.argv) != 3:
        print(f'usage: {sys.argv[0]} <name> <file>')
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])
