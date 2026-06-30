#!/bin/bash

#
# Copyright 2026 Joel Vaz. All rights reserved.
# Licensed under the Apache License 2.0
#
# /proc/$pid/maps need to have "r-xp" permissions
# Uses libssl.so + offset
#
# The offset is the position of 'return ret'
# of 'ssl_fill_hello_random' function
#
# Tested on Arch Linux OpenSSL:
# Version 3.6.2 uses 0x3FFD offset
# Version 3.6.3 uses 0x519D offset
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

OPENSSL_OFFSET=0x519D

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
    local base
    base=$(grep "r-xp" /proc/$pid/maps | grep -m1 libssl.so | awk '{print $1}' | cut -d "-" -f1)
    [[ -n "$base" ]] || { warn "Cannot find libssl.so for PID $pid"; return 1; }
    local dec=$((16#$base + $OPENSSL_OFFSET))
    printf "0x%X\n" "$dec"
}

create_gdb_script() {
    local file="$1"
    local bp_addr="$2"

    cat > "$file" <<EOF
set pagination off
set confirm off
set print thread-events off

define hex32
  set \$p = (unsigned char *)\$arg0
  set \$i = 0
  while \$i < 32
    printf "%02x", \$p[\$i]
    set \$i++
  end
  printf "\n"
end

define patch32
  set \$dst = (unsigned char *)\$arg0
  set \$src = (unsigned char *)\$arg1
  set \$i = 0
  while \$i < 32
    set \$dst[\$i] = \$src[\$i]
    set \$i++
  end
end

define copy32
  set \$dst = (unsigned char *)\$arg0
  set \$src = (unsigned char *)\$arg1
  set \$from = \$arg2
  set \$i = 0
  while \$i < 32
    set \$dst[\$i] = \$src[\$from + \$i]
    set \$i++
  end
end

set \$buf = (unsigned char *) calloc(1, \$filesize)
restore data binary \$buf 0 \$filesize

printf "\nInjected random value:\n"
set \$data = (unsigned char *) malloc(32)
copy32 \$data \$buf 192
x/32xb \$data

printf "\n"
break *$bp_addr
commands
  silent
  printf "\n[Original ServerHello random]\n"
  hex32 result
  patch32 result \$data
  printf "\n[Modified ServerHello random]\n"
  hex32 result
  continue
end

printf "\nReady (waiting for connections)...\n\n"
continue
EOF
}

attach_first_worker() {
    local pid="$1"
    local window="$pid"
    local bp
    bp=$(get_ssl_breakpoint "$pid") || return

    info "Attaching to worker PID $pid"

    local gdb_script
    gdb_script=$(mktemp)
    create_gdb_script "$gdb_script" "$bp"

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
    bp=$(get_ssl_breakpoint "$pid") || return

    info "Attaching to worker PID $pid"

    local gdb_script
    gdb_script=$(mktemp)
    create_gdb_script "$gdb_script" "$bp"

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

    FILESIZE=$(wc -c < data)
    declare -A ATTACHED_PIDS PID_TO_WINDOW GDB_SCRIPTS

    # Attach first worker in initial window
    attach_first_worker "${WORKERS[0]}"

    # Attach remaining workers
    for pid in "${WORKERS[@]:1}"; do
        attach_worker "$pid"
    done

    ok "tmux session created: $SESSION"
    info "Attach: tmux attach -t $SESSION"
    info "Generate traffic: curl -k https://localhost/"

    MASTER=$(pgrep -o -x apache2 2>/dev/null || pgrep -o -x httpd 2>/dev/null) || \
        { warn "Could not find Apache master process"; exit 1; }

    info "Monitoring new and exiting workers..."

    while true; do
        mapfile -t CURRENT_WORKERS < <(pgrep -P "$MASTER" 2>/dev/null || true)

        # Cleanup dead workers
        for pid in "${!ATTACHED_PIDS[@]}"; do
            [[ ! -d "/proc/$pid" || ! " ${CURRENT_WORKERS[*]} " =~ " $pid " ]] && cleanup_pid "$pid"
        done

        # Attach new workers
        for pid in "${CURRENT_WORKERS[@]:-}"; do
            [[ -z "$pid" ]] && continue
            [[ -z "${ATTACHED_PIDS[$pid]:-}" ]] && is_httpd_process "$pid" && attach_worker "$pid"
        done

        sleep 2
    done
}

main "$@"
