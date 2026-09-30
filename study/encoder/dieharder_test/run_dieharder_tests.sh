#!/bin/bash

target="encoder_dieharder"
file=$1

if [[ -z "${file}" ]]; then
  echo "[-] Usage: ./run_dieharder_tests.sh <input>"
  exit 1
fi

if [[ ! -x "${target}" ]]; then
  echo "Error: Target executable $target not found. Make sure to build the project first."
  exit 1
fi

echo "--- Running test case: ${target} ${file} ---"
./${target} ${file} | dieharder -D 262143 -a -g 200 1>${target}.1 2>${target}.2
chmod 400 ${target}.1 ${target}.2

echo "Results for ${target}"
echo "  number of PASSED: " $(grep PASSED ${target}.1 | wc -l)
echo "  number of WEAK: "   $(grep WEAK   ${target}.1 | wc -l)
echo "  number of FAILED: " $(grep FAILED ${target}.1 | wc -l)
