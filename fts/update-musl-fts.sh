#!/bin/sh

FTS_DIR="$(dirname "$(realpath "$0")")"
MUSL_FTS_VERSION=1.2.7
MUSL_FTS_TARBALL_URL="https://github.com/void-linux/musl-fts/archive/refs/tags/v$MUSL_FTS_VERSION.tar.gz"
MUSL_FTS_SRC_DIR="$FTS_DIR/musl-fts"

mkdir -p "$MUSL_FTS_SRC_DIR"

# Download and extract the musl-fts tarball
echo "Downloading musl-fts $MUSL_FTS_VERSION"
curl -L "$MUSL_FTS_TARBALL_URL" | tar xzf - --strip-component=1 -C "$MUSL_FTS_SRC_DIR" \
    musl-fts-$MUSL_FTS_VERSION/fts.c \
    musl-fts-$MUSL_FTS_VERSION/fts.h \
    musl-fts-$MUSL_FTS_VERSION/COPYING

for patch in "$FTS_DIR/patches/"*.patch; do
    echo "Applying patch $patch"
    patch -p1 -d "$MUSL_FTS_SRC_DIR" < "$patch"
done

cat <<EOF > "$MUSL_FTS_SRC_DIR/NOTICE.wasi-libc.md"
This directory contains a copy of the musl-fts library licensed under
BSD-3-Clause with wasi-libc modifications. See COPYING for the full license text.

The original source code can be found at $MUSL_FTS_TARBALL_URL

## How to update musl-fts sources

1. Update the \`MUSL_FTS_VERSION\` variable in \`$0\`
2. Run \`$0\`
EOF
