/*
 * Copyright 2026 Joel Vaz. All rights reserved.
 * Licensed under the Apache License 2.0
 *
 * This code was been generated with AI assistance to
 * automate the process of finding call instructions
 * in x86_64 machine code. Supports direct call
 * instructions (E8 rel32) and indirect call
 * instructions (FF 15 call [rip+disp32]).
 */

#ifndef OPENSSL_OFFSET_X86_SCAN_H
#define OPENSSL_OFFSET_X86_SCAN_H

#include <stddef.h>

/**
 * Scans a function's machine code for the N-th call instruction
 * (E8 or FF 15) targeting a specific virtual address.
 *
 * @param lib_fd Open file descriptor to the ELF file.
 * @param func_vaddr Virtual address of the function start.
 * @param func_size Size of the function in bytes.
 * @param target_vaddr Virtual address of the call target.
 * @param n Which occurrence to find (1-based).
 * @param out_call_vaddr Output for the call instruction's virtual address.
 * @return 0 on success, -1 on failure.
 */
int find_nth_call_to_target(int lib_fd, unsigned long func_vaddr, size_t func_size, unsigned long target_vaddr, int n, unsigned long* out_call_vaddr);

/**
 * Scans a function's machine code for all call instructions
 * (E8 or FF 15) targeting a specific virtual address.
 *
 * Collected call addresses are returned in address (not source) order.
 *
 * @param lib_fd Open file descriptor to the ELF file.
 * @param func_vaddr Virtual address of the function start.
 * @param func_size Size of the function in bytes.
 * @param target_vaddr Virtual address of the call target.
 * @param out_calls Output array for call instruction virtual addresses.
 * @param max_calls Capacity of out_calls.
 * @return The number of calls found, or -1 on failure.
 */
int find_all_calls_to_target(int lib_fd, unsigned long func_vaddr, size_t func_size, unsigned long target_vaddr, unsigned long* out_calls, int max_calls);

#endif /* OPENSSL_OFFSET_X86_SCAN_H */
