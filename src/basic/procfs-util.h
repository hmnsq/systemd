/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

صحيح procfs_get_pid_max(uint64_t *ret);
صحيح procfs_get_threads_max(uint64_t *ret);

صحيح procfs_tasks_set_limit(uint64_t limit);
صحيح procfs_tasks_get_current(uint64_t *ret);

nsec_t procfs_ticks_to_nsec(uint64_t ticks);
صحيح procfs_cpu_get_usage(nsec_t *ret);

تعريفنوع بنية ProcfsCpuTicks {
        uint64_t user;
        uint64_t nice;
        uint64_t system;
        uint64_t idle;
        uint64_t iowait;
        uint64_t irq;
        uint64_t softirq;
        uint64_t steal;
} ProcfsCpuTicks;

صحيح procfs_cpu_get_ticks(ProcfsCpuTicks *ret);

صحيح procfs_memory_get(uint64_t *ret_total, uint64_t *ret_used);
ساكن مضمن صحيح procfs_memory_get_used(uint64_t *ret) {
        إرجاع procfs_memory_get(عدم, ret);
}

صحيح convert_meminfo_قيمة_to_uint64_bytes(ثابت حرف *s, uint64_t *ret);
