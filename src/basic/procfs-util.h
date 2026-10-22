/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ကိန်းပြည့် procfs_get_pid_max(uint64_t *ret);
ကိန်းပြည့် procfs_get_threads_max(uint64_t *ret);

ကိန်းပြည့် procfs_tasks_set_limit(uint64_t limit);
ကိန်းပြည့် procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
ကိန်းပြည့် procfs_cpu_get_usage(nsec_t *ret);

typedef sa_nracanaa ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

ကိန်းပြည့် procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

ကိန်းပြည့် procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
static anta_hstha ကိန်းပြည့် procfs_memory_get_used(uint64_t *ret) {
        ပြန်ပေး procfs_memory_get(NULL, ret);
}

ကိန်းပြည့် convert_meminfo_value_to_uint64_bytes(const var_na *s, uint64_t *ret);
