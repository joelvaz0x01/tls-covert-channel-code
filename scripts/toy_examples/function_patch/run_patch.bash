#!/bin/bash

#
# Copyright 2026 Joel Vaz. All rights reserved.
# Licensed under the Apache License 2.0
#
# Toy example adapted from Tomás Oliveira e Silva, May 2026
#
# As root, do "echo 0 > /proc/sys/kernel/yama/ptrace_scope"
# Attach gdb to a running process using "gdb -p PID"
#

if [[ "$(cat /proc/sys/kernel/yama/ptrace_scope)" != "0" ]]; then
  echo "As root, please run"
  echo "  echo 0 >/proc/sys/kernel/yama/ptrace_scope"
  exit 0
fi

pid=($(pgrep -r R victim_program))
if [[ "$pid" == "" ]]; then
  echo "Victim program not found"
  exit 0
fi
if [[ "${pid[1]}" != "" ]]; then
  echo "More than one victim program found"
  exit 0
fi
echo "PID is $pid"

code_seg=$(grep "r-xp" /proc/$pid/maps | grep victim | cut -d " " -f 1)
code_start=0x$(echo $code_seg | sed -e 's/-.*//')
code_end=0x$(echo $code_seg | sed -e 's/.*-//')
echo "CODE segment is $code_seg (start=$code_start, end+1=$code_end)"

set -e
rm -f script.gdb
grep -B 0 -A 1000 '^# start' run_patch.bash | sed -e "s/CODE_END/$code_end/" >script.gdb

if [[ "$1" == "" ]]; then
  echo "Run this with at least one argument to actually run gdb"
  exit 0
fi
gdb -p $pid -x script.gdb
exit 0

# start of gdb script

print "adjust gdb settings"
set height unlimited

print "use the last 128 bytes of the code segment"
set $code = CODE_END-128
print /x $code

print "allocate the data memory"
set $data = (void *)malloc(4096)
print $data

print "initialize the data memory"
restore data.bin binary $data 0 4095

print "dump the first 256 bytes of the data"
x/256xb $data

print "read the code file"
restore code.bin binary $code 0 127

print "patch the modified_function code"
set *((long *)($code+2)) = $data
set *((int *)($code+0x49)) = (long)original_function-$code-0x47

print "patch the original_function_code"
set *((char *)(original_function+4)) = 0xE9
set *((int *)(original_function+5)) = $code-((long)original_function+9)

print "disassemble the modified code"
disas $code,$code+127

print "disassemble the original code"
disas original_function

print "exit gdb"
exit 0
