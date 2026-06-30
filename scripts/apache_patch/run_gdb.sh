#!/bin/bash

#
# Copyright 2026 Joel Vaz. All rights reserved.
# Licensed under the Apache License 2.0
#
# /proc/$pid/maps need to have "r-xp" permissions
# Uses libssl.so + offset
#
# The offset is the position of the 2nd call of
# 'ret = RAND_bytes_ex()' of 'ssl_fill_hello_random'
# function
#
# Tested on Arch Linux OpenSSL:
# Version 3.6.2 uses 0x3FD6 offset
# Version 3.6.3 uses 0x5176 offset
#
# Apache has two type of processes:
# Parent process  : loads apache
# Child processes : worker processes
#
# The TLS handshake will happen on any worker process
#

set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
BOLD='\033[1m'
NC='\033[0m'

OPENSSL_OFFSET_RAND_CALL=0x5176
OPENSSL_OFFSET_RET=0x519D
DATA='data.bin'
MOD_CODE='code.bin'

header() { echo -e "${MAGENTA}${BOLD}\n==== APACHE HTTPS HANDSHAKE TRACER ===\n${NC}"; }
ok()     { echo -e "${GREEN}[OK]${NC} $1"; }
info()   { echo -e "${BLUE}[INFO]${NC} $1"; }
warn()   { echo -e "${RED}[WARN]${NC} $1"; }

find_httpd_binary() {
  for b in /usr/sbin/httpd /usr/bin/httpd /usr/sbin/apache2 /usr/bin/apache2; do
    [ -x "$b" ] && { echo "$b"; return; }
  done
  command -v httpd 2>/dev/null || command -v apache2 2>/dev/null
}

is_httpd_process() {
  local pid="$1"
  [ -d "/proc/$pid" ] || return 1
  local exe
  exe=$(readlink -f "/proc/$pid/exe" 2>/dev/null || true)
  [[ "$exe" =~ (httpd|apache2)$ ]]
}

collect_workers() {
  local master
  master=$(
    pgrep -o -x apache2 2>/dev/null ||
    pgrep -o -x httpd 2>/dev/null
  ) || return 1

  pgrep -P "$master" | while read -r pid; do
    is_httpd_process "$pid" && echo "$pid"
  done
}

get_ssl_breakpoint() {
  local pid="$1"
  local offset="$2"
  local base
  base=$(grep "r-xp" /proc/$pid/maps | grep -m1 libssl.so | awk '{print $1}' | cut -d "-" -f1)
  [[ -n "$base" ]] || { warn "Cannot find libssl.so for PID $pid"; return 1; }
  local dec=$((16#$base + $offset))
  printf "0x%X\n" "$dec"
}

create_gdb_script() {
  local file="$1"
  local bp_addr="$2"
  local inject_addr="$3"
  local shm_file="$4"
  local mod_code="$5"

  cat > "$file" <<EOF
set pagination off
set confirm off
set print thread-events off

# Loading code.bin
set \$code = (unsigned char *) mmap(0, \$filesize, 7, 0x22, -1, 0)
restore $mod_code binary \$code 0 \$filesize

# Resolving function addresses
set \$real_rand = (void*) RAND_bytes_ex
set \$real_open = (void*) open
set \$real_read = (void*) read
set \$real_close = (void*) close

# Allocating data storage
set \$data_page = (unsigned char *) mmap(0, 4096, 3, 0x22, -1, 0)
set \$shm_fd = (int) open("$shm_file", 2)
set \$shm_size = 4104
set \$shm_ptr = (unsigned char *) mmap(0, \$shm_size, 3, 1, \$shm_fd, 0)
call (int) close(\$shm_fd)
set *(unsigned long long*)\$data_page = (unsigned long long)\$shm_ptr

# Scanning for call instruction
set \$call_found = 0
set \$scan = $inject_addr
set \$limit = $bp_addr + 5
while \$scan < \$limit
  set \$op = *(unsigned char*)\$scan
  if \$op == 0xe8 || \$op == 0xe9
    set \$call_found = 1
    loop_break
  end
  if \$op == 0xff
    set \$next = *(unsigned char*)(\$scan + 1)
    if \$next == 0x15
      set \$call_found = 2
      loop_break
    end
    if \$next == 0x25
      set \$call_found = 3
      loop_break
    end
  end
  set \$scan++
end

# Patching data references in code.bin
set *(int*)(\$code + 0x3) = (int)((long)\$data_page - (long)(\$code + 0x7))

# Patching function calls in code.bin
set *(int*)(\$code + 0x157) = (int)((long)\$real_rand - (long)(\$code + 0x15b))

# Patch call instruction on main
if \$call_found
  set \$new_rel32 = (int)((long)\$code - (long)(\$scan + 5))

  if \$call_found == 1
    set *(int*)(\$scan + 1) = \$new_rel32
  end
  if \$call_found == 2
    set *(unsigned char*)\$scan = 0xe8
    set *(int*)(\$scan + 1) = \$new_rel32
    set *(unsigned char*)(\$scan + 5) = 0x90
  end
  if \$call_found == 3
    set *(unsigned char*)\$scan = 0xe9
    set *(int*)(\$scan + 1) = \$new_rel32
    set *(unsigned char*)(\$scan + 5) = 0x90
  end
end

detach
shell sleep infinity
EOF
}

attach_first_worker() {
  local pid="$1"
  local window="$pid"
  local bp
  bp=$(get_ssl_breakpoint "$pid" "$OPENSSL_OFFSET_RET") || return
  inject=$(get_ssl_breakpoint "$pid" "$OPENSSL_OFFSET_RAND_CALL") || return

  info "Attaching to worker PID $pid"

  local gdb_script
  gdb_script=$(mktemp)
  create_gdb_script "$gdb_script" "$bp" "$inject" "$SHM_FILE" "$MOD_CODE"

  tmux new-session -d -s "$SESSION" -n "$window" \
    "gdb -q \
      -iex 'set debuginfod enabled off' \
      -iex 'set pagination off' \
      -iex 'set confirm off' \
      -iex 'set verbose off' \
      -iex 'set print thread-events off' \
      -ex 'set \$filesize=$FILESIZE' \
      -p $pid \
      -x $gdb_script"

  ATTACHED_PIDS["$pid"]=1
  PID_TO_WINDOW["$pid"]="$window"
  GDB_SCRIPTS["$pid"]="$gdb_script"
}

attach_worker() {
  local pid="$1"
  local window="$pid"
  local bp
  bp=$(get_ssl_breakpoint "$pid" "$OPENSSL_OFFSET_RET") || return
  inject=$(get_ssl_breakpoint "$pid" "$OPENSSL_OFFSET_RAND_CALL") || return

  info "Attaching to worker PID $pid"

  tmux has-session -t "$SESSION" 2>/dev/null || {
    warn "tmux session $SESSION gone, skipping PID $pid";
    return;
  }

  local gdb_script
  gdb_script=$(mktemp)
  create_gdb_script "$gdb_script" "$bp" "$inject" "$SHM_FILE" "$MOD_CODE"

  tmux new-window -t "$SESSION" -n "$window" \
    "gdb -q \
      -iex 'set debuginfod enabled off' \
      -iex 'set pagination off' \
      -iex 'set confirm off' \
      -iex 'set verbose off' \
      -iex 'set print thread-events off' \
      -ex 'set \$filesize=$FILESIZE' \
      -p $pid \
      -x $gdb_script"

  ATTACHED_PIDS["$pid"]=1
  PID_TO_WINDOW["$pid"]="$window"
  GDB_SCRIPTS["$pid"]="$gdb_script"
}

cleanup_pid() {
  local pid="$1"
  local window="${PID_TO_WINDOW[$pid]:-}"

  [[ -n "$window" ]] && {
    info "Worker PID $pid exited"
    tmux kill-window -t "$SESSION:$window" 2>/dev/null || true
  }

  [[ -n "${GDB_SCRIPTS[$pid]:-}" && -f "${GDB_SCRIPTS[$pid]}" ]] && rm -f "${GDB_SCRIPTS[$pid]}"
  unset ATTACHED_PIDS["$pid"]
  unset PID_TO_WINDOW["$pid"]
  unset GDB_SCRIPTS["$pid"]
}

shutdown() {
  info "Shutting down tracer..."
  for pid in "${!ATTACHED_PIDS[@]}"; do
    cleanup_pid "$pid"
  done

  tmux has-session -t "$SESSION" 2>/dev/null && tmux kill-session -t "$SESSION" 2>/dev/null || true
  rm -f "$SHM_FILE"
  ok "Shutdown complete."
  exit 0
}

trap shutdown SIGINT SIGTERM EXIT

main() {
  header
  [[ "$EUID" -ne 0 ]] && { echo "Run as root."; exit 1; }
  ok "Running as root"

  command -v tmux >/dev/null 2>&1 || { echo "tmux is required."; exit 1; }
  ok "tmux available"

  HTTPD_BIN=$(find_httpd_binary)
  [[ -n "$HTTPD_BIN" ]] || { echo "Could not find httpd/apache2 binary."; exit 1; }
  ok "Apache binary: $HTTPD_BIN"

  mapfile -t WORKERS < <(collect_workers)
  [[ "${#WORKERS[@]}" -gt 0 ]] || { echo "No Apache workers found."; exit 1; }

  ok "Found ${#WORKERS[@]} Apache worker(s)"
  for pid in "${WORKERS[@]}"; do
    exe=$(readlink -f "/proc/$pid/exe")
    printf "  PID %-8s %s\n" "$pid" "$exe"
  done

  SESSION="tls"
  tmux kill-session -t "$SESSION" 2>/dev/null || true

  FILESIZE=$(wc -c < "$MOD_CODE")
  SCRIPT_DIR="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"
  SHM_FILE="/var/log/httpd/shm.bin"
  dd if=/dev/zero bs=8 count=1 of="$SHM_FILE" 2>/dev/null
  cat "$SCRIPT_DIR/$DATA" >> "$SHM_FILE"
  chmod 666 "$SHM_FILE"
  declare -A ATTACHED_PIDS PID_TO_WINDOW GDB_SCRIPTS

  attach_first_worker "${WORKERS[0]}"

  for pid in "${WORKERS[@]:1}"; do
    attach_worker "$pid"
  done

  ok "tmux session created: $SESSION"
  info "Attach: tmux attach -t $SESSION"
  info "Generate traffic: curl -k https://localhost/"
  info "Wireshark filter: ssl.handshake.type == 2"

  MASTER=$(pgrep -o -x apache2 2>/dev/null || pgrep -o -x httpd 2>/dev/null) || \
    { warn "Could not find Apache master process"; exit 1; }

  info "Monitoring new and exiting workers..."

  while true; do
    if [[ ! -d "/proc/$MASTER" ]]; then
      info "Apache master process $MASTER exited"
      for pid in "${!ATTACHED_PIDS[@]}"; do
        cleanup_pid "$pid"
      done
      break
    fi

    mapfile -t CURRENT_WORKERS < <(pgrep -P "$MASTER" 2>/dev/null || true)

    for pid in "${!ATTACHED_PIDS[@]}"; do
      [[ ! -d "/proc/$pid" || ! " ${CURRENT_WORKERS[*]} " =~ " $pid " ]] && cleanup_pid "$pid"
    done

    for pid in "${CURRENT_WORKERS[@]:-}"; do
      [[ -z "$pid" ]] && continue
      [[ -z "${ATTACHED_PIDS[$pid]:-}" ]] && is_httpd_process "$pid" && attach_worker "$pid"
    done

    sleep 2
  done
}

main "$@"
