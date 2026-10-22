/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ціле procfs_get_pid_max(uint64_t *ret);
ціле procfs_get_threads_max(uint64_t *ret);

ціле procfs_tasks_set_limit(uint64_t limit);
ціле procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
ціле procfs_cpu_get_usage(nsec_t *ret);

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

ціле procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

ціле procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
static inline ціле procfs_memory_get_used(uint64_t *ret) {
        повернути procfs_memory_get(NULL, ret);
}

ціле convert_meminfo_value_to_uint64_bytes(const char *s, uint64_t *ret);
