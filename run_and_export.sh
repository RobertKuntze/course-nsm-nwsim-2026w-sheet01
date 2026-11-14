#!/usr/bin/env bash
set -e

make MODE=debug

out/clang-debug/Store_dbg -u Cmdenv

rm -f -- ./results.csv
opp_scavetool export -F CSV-S -o results.csv results/*.vec

cp results.csv "results/$(git branch --show-current | sed 's|.*/||').csv"
