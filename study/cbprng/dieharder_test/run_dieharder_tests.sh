#!/bin/bash

TEST_CASES=("4 8" "4 12" "4 16" "4 20" "4 24" "4 28" "8 8" "8 12" "8 16")

run_test_and_report() {
  local b=$1
  local l=$2
  local target="cbprng_dieharder_${b}_${l}"
  local out_file=$(printf "%02d_%02d" $b $l)

  if [[ ! -x "$target" ]]; then
    echo "Error: Target executable $target not found. Make sure to build the project first." >&2
    exit 1
  fi

  if [[ ! -e "${out_file}.1" ]]; then
    echo "--- Running test case: $b, $l ---"
    ./$target | dieharder -D 262143 -a -g 200 1>${out_file}.1 2>${out_file}.2
    chmod 400 ${out_file}.1 ${out_file}.2
  fi

  echo "S_BOX_BITS is $b"
  echo "N_LAYERS is $l"
  echo "  number of PASSED: " $(grep PASSED ${out_file}.1 | wc -l)
  echo "  number of WEAK: "   $(grep WEAK   ${out_file}.1 | wc -l)
  echo "  number of FAILED: " $(grep FAILED ${out_file}.1 | wc -l)
}

for c in "${TEST_CASES[@]}"; do
  set -- $c
  run_test_and_report "$1" "$2" &
done

wait

echo "--- All tests completed ---"
