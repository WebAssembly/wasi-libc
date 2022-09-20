# -*- encoding: utf-8 -*-
"""Generate import file for a target

This utility scans a static library to identify the undefined symbols which are
externally visible which it expects to have provided.  This is used to generate
the import file definitions for WASI.

Example:
    $ python gen-imports.py --nm llvm-nm --prefix __wasi_ libc.a
"""

import argparse
import os
import re
import subprocess
import sys

from subprocess import Popen
from typing import Iterator, List


class SymbolsProvider:
    def __init__(self, nm: str, lib_files: List[str]) -> None:
        self._nm = nm
        self._lib_files = lib_files

    def _get_symbols(self, args: List[str]) -> List[str]:
        process = Popen([self._nm] + args + ['--just-symbol-name', '--extern-only'] + self._lib_files,
                        stdout=subprocess.PIPE)
        output, error = process.communicate()

        lines = output.decode('utf-8').splitlines()
        return sorted(set([line for line in lines if not line.endswith(':') and line]))

    def get_libc_imports_symbols(self, prefix: str) -> Iterator[str]:
        re_prefix = re.compile(prefix)
        symbols = self._get_symbols([])
        return filter(lambda symbol: re_prefix.match(symbol), symbols)

    def get_defined_symbols(self) -> List[str]:
        return self._get_symbols(['--defined-only'])

    def get_undefined_symbols(self) -> List[str]:
        undefined_symbols = self._get_symbols(['--undefined-only'])
        defined_symbols = set(self.get_defined_symbols())
        return [
            symbol for symbol in undefined_symbols
            if symbol not in defined_symbols and not symbol.startswith('__mul')
        ]


def write_symbols(file_path: str, symbols: List[str]) -> None:
    with open(file_path, 'w') as f:
        f.write('\n'.join(symbols))


def main(argv) -> None:
    parser = argparse.ArgumentParser('gen-imports')
    parser.add_argument('--nm', default='nm')
    parser.add_argument('--prefix', default='^_(.*?)imported_wasi_')
    parser.add_argument('--libc-imports-output', required=True)
    parser.add_argument('--defined-symbols-output', required=True)
    parser.add_argument('--undefined-symbols-output', required=True)

    args, unparsed = parser.parse_known_args()

    args.nm = os.path.normpath(args.nm)
    args.libc_imports_output = os.path.normpath(args.libc_imports_output)
    args.defined_symbols_output = os.path.normpath(args.defined_symbols_output)
    args.undefined_symbols_output = os.path.normpath(
        args.undefined_symbols_output)

    lib_files = [os.path.normpath(path) for path in unparsed]

    provider = SymbolsProvider(args.nm, lib_files)

    write_symbols(args.libc_imports_output,
                  provider.get_libc_imports_symbols(args.prefix))
    write_symbols(args.defined_symbols_output, provider.get_defined_symbols())
    write_symbols(args.undefined_symbols_output,
                  provider.get_undefined_symbols())


if __name__ == '__main__':
    main(sys.argv)
