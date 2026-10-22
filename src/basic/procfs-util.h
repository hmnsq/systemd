/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

整数 procfs_get_pid_max(uint64_t *ret);
整数 procfs_get_threads_max(uint64_t *ret);

整数 procfs_tasks_set_limit(uint64_t limit);
整数 procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
整数 procfs_cpu_get_usage(nsec_t *ret);

typedef struct ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

整数 procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

整数 procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
static inline 整数 procfs_memory_get_used(uint64_t *ret) {
        戻る procfs_memory_get(NULL, ret);
}

整数 convert_meminfo_value_to_uint64_bytes(const char *s, uint64_t *ret);
