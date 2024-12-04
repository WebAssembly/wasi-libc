#!/usr/bin/env python

# Filter out test files that do not match a configuration variable.
#
# This script accepts a list of files as arguments and looks for a comment like
# the following in each: `//! filter.py(<VARIABLE>): <EXPRESSION>`. The
# `VARIABLE` value is looked up in the environment and compared with the
# expression. An `EXPRESSION` is a comma-separated list of terms: either
# `<string>` or `!<string>`. If the `VARIABLE` value matches the expression, the
# file is printed; otherwise, it is filtered out.
#
# Example:
# ```
# $ head one.c
# //! filter.py(FOO): bar
# $ head two.c
# //! filter.py(FOO):!bar, baz
# $ FOO=bar ./filter.py one.c two.c
# one.c
# $ FOO=baz ./filter.py one.c two.c
# two.c
# $ ./filter.py one.c two.c
# one.c two.c
# ```

import sys
import os
import re
import logging


def parse_expression(expr):
    """
    An expression is a comma-separated list of terms: either <string> or
    !<string>.
    """
    return [s.strip() for s in expr.split(',')]


def matches_expression(value, expr, pos):
    """
    A value matches an expression if it is in the list of strings or if it is
    not explicitly rejected; order matters, because the first term that matches,
    wins.
    """
    for e in expr:
        if e.startswith('!') and value == e[1:]:
            logging.debug(f'{pos} {value} != {expr}, REJECTED')
            return False
        elif value == e:
            logging.debug(f'{pos} {value} == {expr}, ALLOWED')
            return True
    logging.debug(f'{pos} {value} not in {expr}, REJECTED')
    return False


"""
Match a C comment like the following: `//! filter.py(<VARIABLE>): <EXPRESSION>`.
"""
PATTERN = re.compile('\\s*//\\!\\s*filter\\.py\\(([^)]+)\\):\\s*(.*)')


def line_matches(line, env, pos):
    match = PATTERN.match(line)
    if match:
        value = env.get(match[1])
        if value is None:
            logging.debug(f'{pos} no value for {match[1]}, ALLOWED')
            return True
        else:
            expr = parse_expression(match[2])
            return matches_expression(value, expr, pos)
    return True


def file_matches(file, env):
    with open(file, 'r') as f:
        print = True
        for lineno, line in enumerate(f, start=1):
            print &= line_matches(line, env, f'[{file}:{lineno}]')
        return print


def main(env, files):
    filtered = [file for file in files if file_matches(file, env)]
    print(' '.join(filtered))


if __name__ == "__main__":
    logging.getLogger().name = os.path.basename(__file__)
    if os.environ.get('VERBOSE'):
        logging.basicConfig(level=logging.DEBUG)
    main(os.environ, sys.argv[1:])
