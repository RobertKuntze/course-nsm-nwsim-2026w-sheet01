#!/usr/bin/env bash
set -e

make MODE=debug

out/clang-debug/Store_dbg -u Cmdenv

opp_scavetool export -F CSV-S -o results.csv results/*.vec
