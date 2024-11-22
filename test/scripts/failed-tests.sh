#!/usr/bin/env bash

# This script checks for failed tests in the given directory. Failed tests will
# include text in a *.log file.
#
# Usage: failed-tests.sh [TESTDIR]
#
# Optionally set the VERBOSE environment variable to see the list of failed
# tests.

set -e

TESTDIR=${1:-.}
FAILED=$(find $TESTDIR -type f -and -name *.log -and -not -size 0)

if [[ -n "$VERBOSE" ]]; then
    for failed in "$FAILED"; do
        echo "$failed";
    done
fi

if [[ -n "$FAILED" ]]; then
    exit 1
fi
