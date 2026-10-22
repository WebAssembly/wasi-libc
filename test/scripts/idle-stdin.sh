#!/bin/sh
# Runs the test engine ("$@") with stdin a pipe whose writer stays open, and
# silent, for 3 seconds: a read of stdin has nothing to return until then.
sleep 3 | "$@"
